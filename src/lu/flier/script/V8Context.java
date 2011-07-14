package lu.flier.script;

public class V8Context 
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
	
	@Override
	protected void finalize() throws Throwable 
	{
		super.finalize();
		
		this.internalRelease(this.ctxt);
		
		this.ctxt = 0;
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
	
	public V8Object getGlobal()
	{
		return internalGetGlobal(this.ctxt);
	}	

	private native static long internalCreate();
	private native void internalRelease(long ctxt);
	private native void internalEnter(long ctxt);
	private native void internalLeave(long ctxt);
	private native V8Object internalGetGlobal(long ctxt);
}
