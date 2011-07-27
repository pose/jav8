package lu.flier.script;

public interface V8ContextAware {
	V8Context getContext();
	
	Object bindTo(V8Context ctxt);	
}
