package lu.flier.script;

import java.util.AbstractList;


public class V8Array extends AbstractList<Object> implements V8ContextAware {	
	private final ManagedV8Object array;
    private long obj;
	
	private V8Context ctxt;
	
	V8Array(long obj) {
		this.array = new ManagedV8Object(obj);
		this.ctxt = V8Context.getCurrent();
        this.obj = obj;
	}

	@Override
	public V8Context getContext() {
		return this.ctxt;
	}

	@Override
	public Object bindTo(V8Context ctxt) {
		this.ctxt = ctxt;
		
		return this;
	}
	
	@Override
	public Object get(int index) {
		return ctxt.bind(internalGet(array.obj, index));
	}

	@Override
	public int size() {
		return internalGetSize(array.obj);
	}

    @Override
    public Object[] toArray() {
        return internalToObjectArray(array.obj);
    }

    public void setElements(Object[] elements) {
        this.internalSetElements(this.obj, elements);
    }

	private native Object internalGet(long obj, int index);
	private native void internalSetElements(long obj, Object[] elements);
	private native int internalGetSize(long obj);
	private native Object[] internalToObjectArray(long obj);
}
