package lu.flier.script;

import javax.script.CompiledScript;
import javax.script.ScriptContext;
import javax.script.ScriptEngine;
import javax.script.ScriptException;

public class V8CompiledScript extends CompiledScript
{
	private final V8ScriptEngine engine;
	private final V8Context ctxt;
	
	private long data; 
	
	V8CompiledScript(V8ScriptEngine engine, V8Context ctxt, String script) throws Exception 
	{
		this.engine = engine;		
		this.ctxt = ctxt;
		
		this.data = internalCompile(script);
	}

	@Override
	protected void finalize() throws Throwable {
		super.finalize();
		
		this.internalRelease(this.data);	
		
		this.data = 0;
	}
	
	private native long internalCompile(String String) throws Exception;
	
	private native void internalRelease(long ptr);
	
	private native Object internalExecute(long compiledScript, ScriptContext context) throws Exception;
	
    @Override
    public Object eval(ScriptContext context) throws ScriptException
    {
    	this.ctxt.enter();
    	
    	try {
			return this.internalExecute(this.data, context);
		} catch (Exception e) {
			throw new ScriptException(e);
		} finally {
			this.ctxt.leave();
		}
    }

    @Override
    public ScriptEngine getEngine()
    {
        return this.engine;
    }
}
