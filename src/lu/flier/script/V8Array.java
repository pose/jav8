package lu.flier.script;

import java.util.AbstractList;
import java.util.Date;


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

    public int[] toIntArray() {
        int[] buf = new int[this.size()];
        internalToIntArray(array.obj, buf);
        return buf;
    }

    public long[] toLongArray() {
        long[] buf = new long[this.size()];
        internalToLongArray(array.obj, buf);
        return buf;
    }

    public short[] toShortArray() {
        short[] buf = new short[this.size()];
        internalToShortArray(array.obj, buf);
        return buf;
    }

    public double[] toDoubleArray() {
        double[] buf = new double[this.size()];
        internalToDoubleArray(array.obj, buf);
        return buf;
    }

    public float[] toFloatArray() {
        float[] buf = new float[this.size()];
        internalToFloatArray(array.obj, buf);
        return buf;
    }

    public boolean[] toBooleanArray() {
        boolean[] buf = new boolean[this.size()];
        internalToBooleanArray(array.obj, buf);
        return buf;
    }

    public String[] toStringArray() {
        String[] buf = new String[this.size()];
        internalToStringArray(array.obj, buf);
        return buf;
    }

    public Date[] toDateArray() {
        Date[] buf = new Date[this.size()];
        internalToDateArray(array.obj, buf);
        return buf;
    }

	public void release() {
        this.array.release();
    }

    public void setElements(Object[] elements) {
        this.internalSetElements(this.obj, elements);
    }

    public void setElements(int[] elements) {
        this.internalSetIntElements(this.obj, elements);
    }

    public void setElements(long[] elements) {
        this.internalSetLongElements(this.obj, elements);
    }

    public void setElements(short[] elements) {
        this.internalSetShortElements(this.obj, elements);
    }

    public void setElements(double[] elements) {
        this.internalSetDoubleElements(this.obj, elements);
    }

    public void setElements(float[] elements) {
        this.internalSetFloatElements(this.obj, elements);
    }

    public void setElements(boolean[] elements) {
        this.internalSetBooleanElements(this.obj, elements);
    }

    public void setElements(Date[] elements) {
        this.internalSetDateElements(this.obj, elements);
    }

    public void setElements(V8Array[] elements) {
        this.internalSetV8ArrayElements(this.obj, elements);
    }

    public void setElements(V8Object[] elements) {
        this.internalSetV8ObjectElements(this.obj, elements);
    }

    public void setElements(String[] elements) {
        this.internalSetStringElements(this.obj, elements);
    }

	private native Object internalGet(long obj, int index);
	private native void internalSetElements(long obj, Object[] elements);
	private native void internalSetStringElements(long obj, Object[] elements);
    private native void internalSetIntElements(long obj, int[] elements);
    private native void internalSetLongElements(long obj, long[] elements);
    private native void internalSetShortElements(long obj, short[] elements);
    private native void internalSetDoubleElements(long obj, double[] elements);
    private native void internalSetFloatElements(long obj, float[] elements);
    private native void internalSetBooleanElements(long obj, boolean[] elements);
    private native void internalSetDateElements(long obj, Date[] elements);
    private native void internalSetV8ArrayElements(long obj, V8Array[] elements);
    private native void internalSetV8ObjectElements(long obj, V8Object[] elements);
	private native int internalGetSize(long obj);
	private native Object[] internalToObjectArray(long obj);
    private native int[] internalToIntArray(long obj, int[] buf);
    private native long[] internalToLongArray(long obj, long[] buf);
    private native short[] internalToShortArray(long obj, short[] buf);
    private native double[] internalToDoubleArray(long obj, double[] buf);
    private native float[] internalToFloatArray(long obj, float[] buf);
    private native boolean[] internalToBooleanArray(long obj, boolean[] buf);
    private native String[] internalToStringArray(long obj, String[] buf);
    private native Date[] internalToDateArray(long obj, Date[] buf);
}
