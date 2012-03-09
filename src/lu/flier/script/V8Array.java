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
	public Object set(int index, Object value) {
		return internalSet(array.obj, index, value);
	}

	@Override
	public int size() {
		return internalGetSize(array.obj);
	}

    @Override
    public Object[] toArray() {
        return internalToObjectArray(array.obj, this.size());
    }

    public int[] toIntArray(int length) {
        return toIntArray(new int[length], length);
    }

    public int[] toIntArray(int[] buf, int length) {
        internalToIntArray(array.obj, buf, length);
        return buf;
    }

    public int[] toIntArray() {
        return toIntArray(this.size());
    }

    public long[] toLongArray(int length) {
        return toLongArray(new long[length], length);
    }

    public long[] toLongArray(long[] buf, int length) {
        internalToLongArray(array.obj, buf, length);
        return buf;
    }

    public long[] toLongArray() {
        return toLongArray(this.size());
    }

    public short[] toShortArray(int length) {
        return toShortArray(new short[length], length);
    }

    public short[] toShortArray(short[] buf, int length) {
        internalToShortArray(array.obj, buf, length);
        return buf;
    }

    public short[] toShortArray() {
        return toShortArray(this.size());
    }

    public double[] toDoubleArray(int length) {
        return toDoubleArray(new double[length], length);
    }

    public double[] toDoubleArray(double[] buf, int length) {
        internalToDoubleArray(array.obj, buf, length);
        return buf;
    }

    public double[] toDoubleArray() {
        return toDoubleArray(this.size());
    }

    public float[] toFloatArray(int length) {
        return toFloatArray(new float[length], length);
    }

    public float[] toFloatArray(float[] buf, int length) {
        internalToFloatArray(array.obj, buf, length);
        return buf;
    }

    public float[] toFloatArray() {
        return toFloatArray(this.size());
    }

    public boolean[] toBooleanArray(int length) {
        return toBooleanArray(new boolean[length], length);
    }

    public boolean[] toBooleanArray(boolean[] buf, int length) {
        internalToBooleanArray(array.obj, buf, length);
        return buf;
    }

    public boolean[] toBooleanArray() {
        return toBooleanArray(this.size());
    }

    public String[] toStringArray(int length) {
        return toStringArray(new String[length], length);
    }

    public String[] toStringArray(String[] buf, int length) {
        internalToStringArray(array.obj, buf, length);
        return buf;
    }

    public String[] toStringArray() {
        return toStringArray(this.size());
    }

    public Date[] toDateArray(int length) {
        return toDateArray(new Date[length], length);
    }

    public Date[] toDateArray(Date[] buf, int length) {
        internalToDateArray(array.obj, buf, length);
        return buf;
    }

    public Date[] toDateArray() {
        return toDateArray(this.size());
    }

	public void release() {
        this.array.release();
    }

    public void setElements(Object[] elements, int length) {
        this.internalSetElements(this.obj, elements, length);
    }

    public void setElements(int[] elements, int length) {
        this.internalSetIntElements(this.obj, elements, length);
    }

    public void setElements(long[] elements, int length) {
        this.internalSetLongElements(this.obj, elements, length);
    }

    public void setElements(short[] elements, int length) {
        this.internalSetShortElements(this.obj, elements, length);
    }

    public void setElements(double[] elements, int length) {
        this.internalSetDoubleElements(this.obj, elements, length);
    }

    public void setElements(float[] elements, int length) {
        this.internalSetFloatElements(this.obj, elements, length);
    }

    public void setElements(boolean[] elements, int length) {
        this.internalSetBooleanElements(this.obj, elements, length);
    }

    public void setElements(Date[] elements, int length) {
        this.internalSetDateElements(this.obj, elements, length);
    }

    public void setElements(V8Array[] elements, int length) {
        this.internalSetV8ArrayElements(this.obj, elements, length);
    }

    public void setElements(V8Object[] elements, int length) {
        this.internalSetV8ObjectElements(this.obj, elements, length);
    }

    public void setElements(String[] elements, int length) {
        this.internalSetStringElements(this.obj, elements, length);
    }

    public void setElements(Object[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(int[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(long[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(short[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(double[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(float[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(boolean[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(Date[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(V8Array[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(V8Object[] elements) {
        this.setElements(elements, elements.length);
    }

    public void setElements(String[] elements) {
        this.setElements(elements, elements.length);
    }

	private native Object internalGet(long obj, int index);
	private native Object internalSet(long obj, int index, Object val);
	private native void internalSetElements(long obj, Object[] elements, int length);
	private native void internalSetStringElements(long obj, Object[] elements, int length);
    private native void internalSetIntElements(long obj, int[] elements, int length);
    private native void internalSetLongElements(long obj, long[] elements, int length);
    private native void internalSetShortElements(long obj, short[] elements, int length);
    private native void internalSetDoubleElements(long obj, double[] elements, int length);
    private native void internalSetFloatElements(long obj, float[] elements, int length);
    private native void internalSetBooleanElements(long obj, boolean[] elements, int length);
    private native void internalSetDateElements(long obj, Date[] elements, int length);
    private native void internalSetV8ArrayElements(long obj, V8Array[] elements, int length);
    private native void internalSetV8ObjectElements(long obj, V8Object[] elements, int length);
	private native int internalGetSize(long obj);
	private native Object[] internalToObjectArray(long obj, int length);
    private native int[] internalToIntArray(long obj, int[] buf, int length);
    private native long[] internalToLongArray(long obj, long[] buf, int length);
    private native short[] internalToShortArray(long obj, short[] buf, int length);
    private native double[] internalToDoubleArray(long obj, double[] buf, int length);
    private native float[] internalToFloatArray(long obj, float[] buf, int length);
    private native boolean[] internalToBooleanArray(long obj, boolean[] buf, int length);
    private native String[] internalToStringArray(long obj, String[] buf, int length);
    private native Date[] internalToDateArray(long obj, Date[] buf, int length);
}
