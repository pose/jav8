package lu.flier.script;

class ManagedV8Object {
	protected long obj;
	
	public ManagedV8Object(long obj) {
		this.obj = obj;
	}

	public void release() {
		this.internalRelease(this.obj);		
		
		this.obj = 0;
    }

	@Override
	protected void finalize() throws Throwable {
		super.finalize();
		
		this.internalRelease(this.obj);		
		
		this.obj = 0;
	}
	
	protected native void internalRelease(long ptr);
}
