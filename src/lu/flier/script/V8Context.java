package lu.flier.script;

import javax.script.SimpleScriptContext;

public class V8Context extends SimpleScriptContext
{
	private long ctxt;
	
	V8Context(long ctxt)
	{
		this.ctxt = ctxt;
	}
	
	public V8Context()
	{
		this.ctxt = internalCreate();
	}
	
	public void dispose()
	{
		if (this.ctxt > 0) 
		{			
			this.internalRelease(this.ctxt);
			
			this.ctxt = 0;
		}
	}
	
	
	@Override
	protected void finalize() throws Throwable 
	{
		super.finalize();
		
		this.dispose();
	}
	
	public native static V8Context getEntered();
	public native static V8Context getCurrent();
	public native static V8Context getCalling();
	public native static boolean inContext();
	
	public void enter()
	{
		internalEnter(this.ctxt);
	}
	
	public void leave()
	{
		internalLeave(this.ctxt);
	}
	
	public Object bind(Object obj) 
	{
		return obj instanceof V8ContextAware ? ((V8ContextAware) obj).bindTo(this) : obj;
	}
	
	public V8Object getGlobal()
	{
		return (V8Object) internalGetGlobal(this.ctxt).bindTo(this);
	}	

	private native static long internalCreate();
	private native void internalRelease(long ctxt);
	private native void internalEnter(long ctxt);
	private native void internalLeave(long ctxt);
	private native V8Object internalGetGlobal(long ctxt);
}
