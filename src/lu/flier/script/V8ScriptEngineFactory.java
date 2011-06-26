package lu.flier.script;

import java.util.ArrayList;
import java.util.List;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineFactory;

@SuppressWarnings("serial")
public class V8ScriptEngineFactory implements ScriptEngineFactory
{
    private static List<String> names;
    private static List<String> mimeTypes;
    private static List<String> extensions;

    static
    {
        names = new ArrayList<String>()
        {
            {
                add("js");
                add("v8");
                add("jav8");
                add("JavaScript");
                add("javascript");
                add("ECMAScript");
                add("ecmascript");
            }
        };

        mimeTypes = new ArrayList<String>()
        {
            {
                add("application/javascript");
                add("application/ecmascript");
                add("text/javascript");
                add("text/ecmascript");
            }
        };

        extensions = new ArrayList<String>()
        {
            {
                add("js");
            }
        };
    }

    public String getName()
    {
        return (String) getParameter(ScriptEngine.NAME);
    }

    @Override
    public String getEngineName()
    {
        return (String) getParameter(ScriptEngine.ENGINE);
    }

    @Override
    public String getEngineVersion()
    {
        return (String) getParameter(ScriptEngine.ENGINE_VERSION);
    }

    @Override
    public List<String> getExtensions()
    {
        return extensions;
    }

    @Override
    public List<String> getMimeTypes()
    {
        return mimeTypes;
    }

    @Override
    public List<String> getNames()
    {
        return names;
    }

    @Override
    public String getLanguageName()
    {
        return (String) getParameter(ScriptEngine.LANGUAGE);
    }

    @Override
    public String getLanguageVersion()
    {
        return (String) getParameter(ScriptEngine.LANGUAGE_VERSION);
    }

    @Override
    public native Object getParameter(String key);

    @Override
    public String getMethodCallSyntax(String obj, String m, String... args)
    {
        StringBuilder sb = new StringBuilder(obj).append('.').append(m).append('(');

        for (String arg : args)
        {
            if (sb.charAt(sb.length() - 1) != '(') sb.append(',');

            sb.append(arg);
        }

        return sb.append(')').toString();
    }

    @Override
    public String getOutputStatement(String toDisplay)
    {
        StringBuilder sb = new StringBuilder("print(\"");

        sb.append(toDisplay.replace("\\", "\\\\").replace("\"", "\\\""));

        return sb.append("\")").toString();
    }

    @Override
    public String getProgram(String... statements)
    {
        StringBuilder sb = new StringBuilder();

        for (String stmt : statements)
            sb.append(stmt).append(';');

        return sb.toString();
    }

    @Override
    public ScriptEngine getScriptEngine()
    {
        return new V8ScriptEngine(this);
    }

}
