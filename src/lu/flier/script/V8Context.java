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

    public V8Array createArray(Object[] contents, int length) {
        return (V8Array)internalCreateArray(contents, length).bindTo(this);
    }

    public V8Array createArray(int[] contents, int length) {
        return (V8Array)internalCreateIntArray(contents, length).bindTo(this);
    }

    public V8Array createArray(long[] contents, int length) {
        return (V8Array)internalCreateLongArray(contents, length).bindTo(this);
    }

    public V8Array createArray(double[] contents, int length) {
        return (V8Array)internalCreateDoubleArray(contents, length).bindTo(this);
    }

    public V8Array createArray(float[] contents, int length) {
        return (V8Array)internalCreateFloatArray(contents, length).bindTo(this);
    }

    public V8Array createArray(short[] contents, int length) {
        return (V8Array)internalCreateShortArray(contents, length).bindTo(this);
    }

    public V8Array createArray(boolean[] contents, int length) {
        return (V8Array)internalCreateBoolArray(contents, length).bindTo(this);
    }

    public V8Array createArray(String[] contents, int length) {
        return (V8Array)internalCreateStringArray(contents, length).bindTo(this);
    }

    public V8Array createArray(Date[] contents, int length) {
        return (V8Array)internalCreateDateArray(contents, length).bindTo(this);
    }

    public V8Array createArray(V8Array[] contents, int length) {
        return (V8Array)internalCreateV8ArrayArray(contents, length).bindTo(this);
    }

    public V8Array createArray(V8Object[] contents, int length) {
        return (V8Array)internalCreateV8ObjectArray(contents, length).bindTo(this);
    }

    public V8Array createArray(Object[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(int[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(long[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(double[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(float[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(short[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(boolean[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(String[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(Date[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(V8Array[] contents) {
        return createArray(contents, contents.length);
    }

    public V8Array createArray(V8Object[] contents) {
        return createArray(contents, contents.length);
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
    public native V8Array internalCreateArray(Object[] contents, int length);
    public native V8Array internalCreateIntArray(int[] contents, int length);
    public native V8Array internalCreateStringArray(String[] contents, int length);
    public native V8Array internalCreateLongArray(long[] contents, int length);
    public native V8Array internalCreateDoubleArray(double[] contents, int length);
    public native V8Array internalCreateFloatArray(float[] contents, int length);
    public native V8Array internalCreateShortArray(short[] contents, int length);
    public native V8Array internalCreateBoolArray(boolean[] contents, int length);
    public native V8Array internalCreateDateArray(Date[] contents, int length);
    public native V8Array internalCreateV8ArrayArray(V8Array[] contents, int length);
    public native V8Array internalCreateV8ObjectArray(V8Object[] contents, int length);
	
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
