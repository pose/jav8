package lu.flier.script;

import java.io.Reader;

import javax.script.AbstractScriptEngine;
import javax.script.Bindings;
import javax.script.ScriptContext;
import javax.script.ScriptEngineFactory;
import javax.script.ScriptException;

public final class V8ScriptEngine extends AbstractScriptEngine
{
    private final V8ScriptEngineFactory factory;

    V8ScriptEngine(V8ScriptEngineFactory factory)
    {
        assert factory != null;

        this.factory = factory;

        Bindings scope = getBindings(ScriptContext.ENGINE_SCOPE);

        scope.put(ENGINE, factory.getEngineName());
        scope.put(ENGINE_VERSION, factory.getEngineVersion());
        scope.put(NAME, factory.getName());
        scope.put(LANGUAGE, factory.getLanguageName());
        scope.put(LANGUAGE_VERSION, factory.getLanguageVersion());
    }

    @Override
    public Object eval(String script, ScriptContext context) throws ScriptException
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Object eval(Reader reader, ScriptContext context) throws ScriptException
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Bindings createBindings()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public ScriptEngineFactory getFactory()
    {
        return this.factory;
    }

}
