package lu.flier.script;

public class V8Function extends V8Object 
{
	private final V8Object thiz;
	
	public V8Function(V8Object thiz, long obj) {
		super(obj);
		
		this.thiz = thiz;
	}
	
	public Object invoke(Object[] args) {
		return internalInvoke(this.obj, this.thiz != null ? this.thiz.obj : null, args);
	}

	private native Object internalInvoke(long obj, Long thiz, Object[] args);
}
