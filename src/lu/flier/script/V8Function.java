package lu.flier.script;

public class V8Function extends V8Object 
{
	private final V8Object thiz;
	
	public V8Function(long obj) {
		super(obj);
		
		this.thiz = null;
	}
	
	public V8Function(V8Object thiz, long obj) {
		super(obj);
		
		this.thiz = thiz;
	}
	
	public Object invoke(Object... args) {
		Object result = internalInvoke(this.obj, this.thiz != null ? this.thiz.obj : 0, args);
		return result == null ? null : this.ctxt.bind(result);
	}

    /**
     * Invokes a void, argument-less function in a high performance manner.
     */
	public void invokeVoid(Object... args) {
        if (args.length == 0) {
            internalInvokeVoid(this.obj, null);
        } else {
            internalInvokeVoid(this.obj, args);
        }
	}

	private native Object internalInvoke(long obj, long thiz, Object[] args);
	private native void internalInvokeVoid(long obj, Object[] args);
}
