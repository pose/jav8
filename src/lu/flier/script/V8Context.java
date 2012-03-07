package lu.flier.script;

import javax.script.Bindings;
import javax.script.SimpleScriptContext;
import java.util.Date;

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
	
	@Override
	public Bindings getBindings(int scope) {
		if (scope == ENGINE_SCOPE) {
			return getGlobal();
		}

		return super.getBindings(scope);
	}

	@Override
	public void setBindings(Bindings bindings, int scope) {
		if (scope == ENGINE_SCOPE) {
			getGlobal().clear();
		}
		super.setBindings(bindings, scope);
	}

	@Override
	public Object getAttribute(String name, int scope) {
		if (scope == ENGINE_SCOPE) {
			return getGlobal().get(name);
		}
		
		return super.getAttribute(name, scope);
	}

	@Override
	public Object removeAttribute(String name, int scope) {
		if (scope == ENGINE_SCOPE) {
			return getGlobal().remove(name);
		}
		return super.removeAttribute(name, scope);
	}

	@Override
	public int getAttributesScope(String name) {
		if (getGlobal().containsKey(name)) {
            return ENGINE_SCOPE;
		}
		return super.getAttributesScope(name);
	}

	@Override
	public void setAttribute(String name, Object value, int scope) {
		if (scope == ENGINE_SCOPE) {
			getGlobal().put(name, value);
		} else {
			super.setAttribute(name, value, scope);
		}
	}

    public V8Object createObject() {
        return (V8Object)internalCreateObject().bindTo(this);
    }

    public V8Array createArray(Object[] contents) {
        return (V8Array)internalCreateArray(contents).bindTo(this);
    }

    public V8Array createArray(int[] contents) {
        return (V8Array)internalCreateIntArray(contents).bindTo(this);
    }

    public V8Array createArray(long[] contents) {
        return (V8Array)internalCreateLongArray(contents).bindTo(this);
    }

    public V8Array createArray(double[] contents) {
        return (V8Array)internalCreateDoubleArray(contents).bindTo(this);
    }

    public V8Array createArray(float[] contents) {
        return (V8Array)internalCreateFloatArray(contents).bindTo(this);
    }

    public V8Array createArray(short[] contents) {
        return (V8Array)internalCreateShortArray(contents).bindTo(this);
    }

    public V8Array createArray(boolean[] contents) {
        return (V8Array)internalCreateBoolArray(contents).bindTo(this);
    }

    public V8Array createArray(String[] contents) {
        return (V8Array)internalCreateStringArray(contents).bindTo(this);
    }

    public V8Array createArray(Date[] contents) {
        return (V8Array)internalCreateDateArray(contents).bindTo(this);
    }

    public V8Array createArray(V8Array[] contents) {
        return (V8Array)internalCreateV8ArrayArray(contents).bindTo(this);
    }

    public V8Array createArray(V8Object[] contents) {
        return (V8Array)internalCreateV8ObjectArray(contents).bindTo(this);
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
    public native V8Object internalCreateObject();
    public native V8Array internalCreateArray(Object[] contents);
    public native V8Array internalCreateIntArray(int[] contents);
    public native V8Array internalCreateStringArray(String[] contents);
    public native V8Array internalCreateLongArray(long[] contents);
    public native V8Array internalCreateDoubleArray(double[] contents);
    public native V8Array internalCreateFloatArray(float[] contents);
    public native V8Array internalCreateShortArray(short[] contents);
    public native V8Array internalCreateBoolArray(boolean[] contents);
    public native V8Array internalCreateDateArray(Date[] contents);
    public native V8Array internalCreateV8ArrayArray(V8Array[] contents);
    public native V8Array internalCreateV8ObjectArray(V8Object[] contents);
	
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

	private native long internalCreate();
	private native void internalRelease(long ctxt);
	private native void internalEnter(long ctxt);
	private native void internalLeave(long ctxt);
	private native V8Object internalGetGlobal(long ctxt);
}
