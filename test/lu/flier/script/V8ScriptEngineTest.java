package lu.flier.script;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.lang.ref.Reference;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import javax.script.Bindings;
import javax.script.Compilable;
import javax.script.CompiledScript;
import javax.script.Invocable;
import javax.script.ScriptContext;
import javax.script.ScriptEngine;
import javax.script.ScriptEngineFactory;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class V8ScriptEngineTest
{
    private ScriptEngine eng;
    
    private ScriptEngine createScriptEngine()
    {
    	return new ScriptEngineManager().getEngineByName("jav8");
    }

    @Before
    public void setUp()
    {
        this.eng = createScriptEngine();
    }
    
    @After
    public void tearDown()
    {
    	this.eng = null;
    	
    	System.gc();    	
    }

    @Test
    public void testV8ScriptEngineFactory()
    {    	
    	assertNotNull(this.eng);
    	
        ScriptEngineFactory factory = this.eng.getFactory();

        assertNotNull(factory);

        List<String> names = factory.getNames();

        assertTrue(names.contains("jav8"));
        assertTrue(names.contains("javascript"));

        List<String> mimeTypes = factory.getMimeTypes();

        assertTrue(mimeTypes.contains("text/javascript"));

        List<String> extensions = factory.getExtensions();

        assertTrue(extensions.contains("js"));
        
        assertEquals("Google V8", factory.getEngineName());
        assertEquals("ECMAScript", factory.getLanguageName());
        assertEquals("1.8.5", factory.getLanguageVersion());
        
        assertEquals("obj.say(Hello,World)", factory.getMethodCallSyntax("obj", "say", "Hello", "World"));
        assertEquals("print(\"Hello world\")", factory.getOutputStatement("Hello world"));
        assertEquals("s1;s2;", factory.getProgram("s1", "s2"));
    }
        
    @Test
    public void testCompile() throws ScriptException
    {
    	Compilable compiler = (Compilable) this.eng;
    	
    	CompiledScript script = compiler.compile("1+2");
    	
    	assertNotNull(script);
    	
    	assertEquals(this.eng, script.getEngine());
    	assertEquals(3, script.eval());
    	
    	script = compiler.compile(new StringReader("2+3"));
    	assertEquals(5, script.eval());
    }
    
    public interface IHello {
    	String hello(String name);
    }
    
    public interface IPerson {
    	String say(String name);
    }    

    @Test
    public void testInvoke() throws ScriptException, NoSuchMethodException, IOException 
    {
    	Invocable invocable = (Invocable) this.eng; 
    	
    	this.eng.eval("function hello(name) { return 'Hello ' + name; };" +
    				  "function Person(me) {" +
    				  "	 this.say = function (you) {" +
    				  "    return me + ' say, hello ' + you; }" +
    				  "}" +
    				  "var me = new Person('Flier');");
    	    	
    	assertEquals("Hello Flier", invocable.invokeFunction("hello", "Flier"));
    	
    	try {
    		invocable.invokeFunction("nonexist");
    		fail();
    	} catch (NoSuchMethodException e) {    		
    	}
    	    	
    	IHello hello = invocable.getInterface(IHello.class);
    	
    	assertNotNull(hello);
    	assertEquals("Hello Flier", hello.hello("Flier"));
    	
    	V8Context ctxt = ((V8ScriptEngine) this.eng).getV8Context();
    	    	
		V8Object me = (V8Object) ctxt.getGlobal().get("me");
		
		assertNotNull(me);
	
		assertEquals("Flier say, hello baby", invocable.invokeMethod(me, "say", "baby"));
		
		IPerson person = invocable.getInterface(me, IPerson.class);
				
		assertNotNull(person);
		assertEquals("Flier say, hello baby", person.say("baby"));
    }    
    
    @Test
    public void testEngineBindings() throws ScriptException
    {
    	this.eng.eval("var b = true;" +
				  "var i = 123;" +
				  "var n = 3.14;" +
				  "var s = 'test';" +
				  "var n1 = null;" +
				  "var n2 = undefined;" +
				  "var d = new Date();" +
				  "var o = { name: 'Flier' };" +
				  "var a = [1, 2, 3];" + 
				  "var f = function () {};");
    	
    	Bindings g = this.eng.getBindings(ScriptContext.ENGINE_SCOPE);
		
		assertNotNull(g);
		
		Object[] keys = g.keySet().toArray();
		Arrays.sort(keys);		
		assertEquals("[a, b, d, f, i, javax.script.engine, javax.script.engine_version, " +
			"javax.script.language, javax.script.language_version, javax.script.name, " +
			"n, n1, n2, o, s]", Arrays.toString(keys));
		assertEquals(15, g.size());
		assertFalse(g.isEmpty());
		
		
		assertTrue(g.containsKey("b"));
		assertFalse(g.containsKey("nonexist"));
		assertTrue(g.containsValue("test"));
		assertFalse(g.containsValue("nonexist"));
		
		assertEquals(true, g.get("b"));
		assertEquals(123, g.get("i"));
		assertEquals(3.14, g.get("n"));
		assertEquals("test", g.get("s"));
		assertEquals(null, g.get("n1"));
		assertEquals(null, g.get("n2"));
		assertEquals(Date.class, g.get("d").getClass());    		
		assertEquals(V8Object.class, g.get("o").getClass());
		assertEquals("Flier", ((Bindings) g.get("o")).get("name"));
		assertEquals(V8Array.class, g.get("a").getClass());
		assertEquals(V8Function.class, g.get("f").getClass());
		
		assertEquals(null, g.get("test"));
		assertEquals(null, g.put("test", "test"));
		assertEquals("test", g.get("test"));
		assertEquals("test", g.remove("test"));
		assertEquals(null, g.get("test"));
		
		g.clear();
		assertTrue(g.isEmpty());
    }
        
    @Test
    public void testGlobalBindings() throws ScriptException
    {
    	this.eng.eval("var a = 1");
    	
    	Bindings engineScope = this.eng.getBindings(ScriptContext.ENGINE_SCOPE);
    	Bindings globalScope = this.eng.getBindings(ScriptContext.GLOBAL_SCOPE);
    	
    	assertNotNull(engineScope);
    	assertNotNull(globalScope);
    	
    	assertTrue(engineScope.containsKey("a"));
    	assertFalse(globalScope.containsKey("a"));
    	assertFalse(globalScope.containsKey("b"));
    	
    	ScriptEngine engine = createScriptEngine();
    	
    	assertFalse(engine.getBindings(ScriptContext.ENGINE_SCOPE).containsKey("a"));
    	assertFalse(engine.getBindings(ScriptContext.GLOBAL_SCOPE).containsKey("b"));
    }

    @Test
    public void testJavascriptArray() throws ScriptException, NoSuchMethodException 
    {    	
		V8Array array = (V8Array) this.eng.eval("[1, 2, 3]");
		
    	assertNotNull(array);
    	assertEquals(3, array.size());
    	assertEquals(1, array.get(0));
    	assertEquals(null, array.get(4));    		  
    	
    	assertArrayEquals(new Object[] {1, 2, 3}, array.toArray());    	
    }
    
    @Test
    public void testDate() throws ScriptException, NoSuchMethodException
    {
    	Calendar cal = Calendar.getInstance();
    	
    	cal.set(2011, 8-1, 3, 0, 0, 0);
    	cal.set(Calendar.MILLISECOND, 0);
    	
    	Date d = (Date) this.eng.eval("var d = new Date(); d.setTime(Date.parse('2011-8-3')); d");
    	
    	assertEquals(cal.getTime(), d);
    	
    	this.eng.eval("function test(d) { return d.getTime() == Date.parse('2011-8-3'); }");
    	
    	assertTrue((Boolean) ((Invocable) this.eng).invokeFunction("test", cal.getTime()));
    }
    
    @Test
    public void testJavaObjects() throws ScriptException, NoSuchMethodException
    {
    	this.eng.eval("function hello() { return 'hello ' + name; }" +
    				  "function helloPerson() { person.name = 'flier'; return 'hello ' + person.name; };");
    	
    	Bindings engineScope = this.eng.getBindings(ScriptContext.ENGINE_SCOPE);
    	
    	Invocable invocable = (Invocable) this.eng;
    	
    	try {
    		invocable.invokeFunction("hello");
    		
    		fail();
    	} catch (ReferenceError e) {
    		
    	}
    	
    	class Person {
			public String name;
			
			public String hello() {
				return "hello " + this.name; 
			}
    	}
    	
    	Person person = new Person();
    	
    	assertEquals(null, engineScope.put("name", "test"));
    	assertEquals(null, engineScope.put("person", person));
    	
    	assertEquals("hello test", invocable.invokeFunction("hello"));
    	assertEquals("hello flier", invocable.invokeFunction("helloPerson"));
    	assertEquals("flier", ((Person) engineScope.get("person")).name);
    	assertEquals("hello flier", invocable.invokeMethod(engineScope.get("person"), "hello"));
    	
    	// Named getter
    	assertSame(person, this.eng.eval("person"));
    	assertEquals("flier", this.eng.eval("person.name"));
    	assertEquals(null, this.eng.eval("person.test"));
    	
    	// Named setter
    	assertEquals("flier", person.name);
    	assertEquals("test", this.eng.eval("person.name = 'test'"));
    	assertEquals("test", person.name);
    	
    	// Named query
    	assertTrue((Boolean) this.eng.eval("'name' in person"));
    	assertTrue((Boolean) this.eng.eval("'hello' in person"));
    	assertFalse((Boolean) this.eng.eval("'test' in person"));
    	
    	// Named deleter
    	assertEquals("test", person.name);
    	assertEquals(false, this.eng.eval("delete person.name;"));
    	assertEquals(false, this.eng.eval("delete person['name'];"));
    	assertEquals("test", person.name);
    	
    	// Named enumerator
    	V8Array names = (V8Array) this.eng.eval("var names = []; for (var name in person) { names.push(name); } names;");
    	
    	assertNotNull(names);
    	assertEquals(9, names.size());
    	Object[] keys = names.toArray();
    	Arrays.sort(keys);		
    	assertEquals("[equals, getClass, hashCode, hello, name, notify, notifyAll, toString, wait]", Arrays.toString(keys));
    }
    
    @Test
    public void testJavaArray() throws ScriptException, NoSuchMethodException
    {
    	this.eng.eval("function test(array) { " +
				"var n = 0;" +
				"array[1] = 4;" +
				"for (var i=0; i<array.length; i++) n += array[i]; " +
				"return n; }");
		
		int[] array2 = new int[] { 1, 2, 3 };
		
		assertEquals(8, ((Invocable) this.eng).invokeFunction("test", array2));
		assertEquals(4, array2[1]);
		
		Bindings g = this.eng.getBindings(ScriptContext.ENGINE_SCOPE);
		
		g.put("array", array2);
		
		// Indexed getter
		assertEquals(1, this.eng.eval("array[0]"));
		assertEquals(null, this.eng.eval("array[-1]"));
		try {
			this.eng.eval("array[100]");
			fail();
		} catch (ScriptException e) {  
			assertEquals(ArrayIndexOutOfBoundsException.class, e.getCause().getCause().getClass());
		}
		
		// Indexed setter
		assertEquals(4, this.eng.eval("array[1]"));
		assertEquals(2, this.eng.eval("array[1] = 2"));
		assertEquals(2, this.eng.eval("array[1]"));
		try {
			this.eng.eval("array[100] = 100");
			fail();
		} catch (ScriptException e) {  
			assertEquals(ArrayIndexOutOfBoundsException.class, e.getCause().getCause().getClass());
		}
		
		// Indexed query
		assertEquals(true, this.eng.eval("0 in array"));
		assertEquals(true, this.eng.eval("2 in array"));
		assertEquals(false, this.eng.eval("-1 in array"));
		assertEquals(false, this.eng.eval("3 in array"));
		
		// Indexed deleter
		assertEquals(2, this.eng.eval("array[1]"));
		assertEquals(false, this.eng.eval("delete array[1]"));
		assertEquals(2, this.eng.eval("array[1]"));
		
		// Indexed enumerator
    	V8Array indexes = (V8Array) this.eng.eval("var indexes = []; for (var idx in array) { indexes.push(idx); } indexes;");

    	assertEquals(3, indexes.size());
    	assertEquals("[0, 1, 2]", Arrays.toString(indexes.toArray()));
    }
    
    public static void print(String str) {
    	System.out.print(str);
    }
    
    public static void println(String str) {
    	System.out.println(str);
    }
            
	public static String sprintf(String fmt, V8Array args) {		
		return String.format(fmt, args.toArray());
	}
	
    @Test
    public void testJavaFunction() throws ScriptException, SecurityException, NoSuchMethodException
    {
    	class Person {
			public String hello(String name) {
				return "hello " + name; 
			}
    	}
    	
    	Person person = new Person();
    	
    	this.eng.put("person", person);
    	this.eng.put("sprintf", this.getClass().getMethod("sprintf", String.class, V8Array.class));
    	
    	assertEquals("hello flier", this.eng.eval("person.hello('flier')"));
    	assertEquals("hello world, 42", this.eng.eval("sprintf('hello %s, %d', ['world', 42]);"));
    }
    
    @Test
    public void testCodeExamples() throws SecurityException, NoSuchMethodException, ScriptException
    {
    	// Scripting for the Java Platform
    	//
    	// http://java.sun.com/developer/technicalArticles/J2SE/Desktop/scripting/
    	
    	// Code Example 1: Create a ScriptEngine object using the engine name.
    	//
    	ScriptEngineManager mgr = new ScriptEngineManager();
    	ScriptEngine jsEngine = mgr.getEngineByName("jav8");
    	
    	jsEngine.put("print", this.getClass().getMethod("print", String.class));
    	jsEngine.put("println", this.getClass().getMethod("println", String.class));
    
    	jsEngine.eval("print('Hello, world!')");
    	
    	// Code Example 2: You can retrieve a list of all engines installed to your Java platform.
    	//
    	List<ScriptEngineFactory> factories = mgr.getEngineFactories();
    	
    	assertFalse(factories.isEmpty());
    	
    	// Code Example 3: A ScriptEngineFactory object provides detailed information about the engine it provides.
    	//
    	for (ScriptEngineFactory factory: factories) {
    	    System.out.println("ScriptEngineFactory Info");
    	    
    	    String engName = factory.getEngineName();
    	    String engVersion = factory.getEngineVersion();
    	    String langName = factory.getLanguageName();
    	    String langVersion = factory.getLanguageVersion();
    	    System.out.printf("\tScript Engine: %s (%s)\n", engName, engVersion);
    	    
    	    List<String> engNames = factory.getNames();
    	    for(String name: engNames) {
    	      System.out.printf("\tEngine Alias: %s\n", name);
    	    }
    	    System.out.printf("\tLanguage: %s (%s)\n", langName, langVersion);
    	}
    	    	
    	// Code Example 6: The eval method can read script files.
    	//
    	InputStream is = this.getClass().getResourceAsStream("/scripts/F1.js");
    	/*
    	try {
    	    Reader reader = new InputStreamReader(is);
    	    engine.eval(reader);
    	} catch (ScriptException ex) {
    	    ex.printStackTrace();
    	}
    	*/
    	// Code Example 7: You can use the Invocable interface to call specific methods in a script.
    	//
    	jsEngine.eval("function sayHello() {" +
                 	  "  println('Hello, world!');" +
                 	  "}");
    	Invocable invocableEngine = (Invocable) jsEngine;
    	invocableEngine.invokeFunction("sayHello");
    	
    	// Code Example 8: Java programming language code adds names to a list.
    	//
    	List<String> namesList = new ArrayList<String>();
    	
    	namesList.add("Jill");
    	namesList.add("Bob");
    	namesList.add("Laureen");
    	namesList.add("Ed");
    	
    	// Code Example 9: Script code can both access and modify native Java objects.
    	//
    	jsEngine.put("namesListKey", namesList);
    	  
    	System.out.println("Executing in script environment...");
    	
    	jsEngine.eval("var x;" +
	                  "var names = namesListKey.toArray();" +
	                  "for(x in names) {" +
	                  //"  println(names[x]);" +
	                  "}" +
	                  "namesListKey.add('Dana');");
    	
    	System.out.println("Executing in Java environment...");
    	
    	for (String name: namesList) {
    	    System.out.println(name);
    	}  
    	
    	// Code Example 10: Applications can pass values to script using the Invocable interface.
    	//  	    	
	    jsEngine.eval("function printNames1(namesList) {" +
	                  "  var x;" +
	                  "  var names = namesList.toArray();" +
	                  "  for(x in names) {" +
	                  "    println(names[x]);" +
	                  "  }" +
	                  "}" +

	                  "function addName(namesList, name) {" +
	                  "  namesList.add(name);" +
	                  "}");
	    invocableEngine.invokeFunction("printNames1", namesList);
	    invocableEngine.invokeFunction("addName", namesList, "Dana");
    }
	
	class TestObject
	{
		public String name = "flier";
	}
	
    public Reference<TestObject> runObjectTracer() throws ScriptException, NoSuchMethodException
    {    	
    	TestObject obj = new TestObject();
    	
    	ScriptEngine eng = createScriptEngine();
    	
    	eng.eval("function test(o) { var name = o.name; delete o; return name; }");
    	
    	assertEquals("flier", ((Invocable) eng).invokeFunction("test", obj));
    	
    	return new WeakReference<TestObject>(obj);
    }
    
    @Test
    public void testObjectTracer() throws ScriptException, NoSuchMethodException 
    {   	
    	Reference<TestObject> ref = runObjectTracer();
    	
    	assertNotNull(ref.get());
    	    	
    	Runtime.getRuntime().gc();
    	
    	assertNotNull(ref.get());
    	
    	V8ScriptEngine.gc();
    	Runtime.getRuntime().gc();
    	
    	assertNull(ref.get());    	    
    }
}
