package lu.flier.script;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;

import javax.script.Compilable;
import javax.script.CompiledScript;
import javax.script.Invocable;
import javax.script.ScriptEngine;
import javax.script.ScriptEngineFactory;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import org.junit.Before;
import org.junit.Test;

public class V8ScriptEngineTest
{
    private ScriptEngine eng;

    @Before
    public void setUp()
    {
        this.eng = new ScriptEngineManager().getEngineByName("jav8");
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
    
    @Test
    public void testInvoke() throws ScriptException, NoSuchMethodException, IOException 
    {
    	Invocable invocable = (Invocable) this.eng; 
    	
    	this.eng.eval("function hello(name) { return 'Hello ' + name; }");
    	    	
    	assertEquals("Hello Flier", invocable.invokeFunction("hello", "Flier"));
    	
    	try {
    		invocable.invokeFunction("nonexist");
    		fail();
    	} catch (NoSuchMethodException e) {
    		
    	}
    		
    }    
}
