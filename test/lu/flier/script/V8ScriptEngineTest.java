package lu.flier.script;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.util.List;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineFactory;
import javax.script.ScriptEngineManager;

import org.junit.Before;
import org.junit.Test;

public class V8ScriptEngineTest
{
    private ScriptEngine eng;

    @Before
    public void setUp()
    {
        ScriptEngineManager mgr = new ScriptEngineManager();
        this.eng = mgr.getEngineByName("jav8");
    }

    @Test
    public void testV8ScriptEngineFactory()
    {
        ScriptEngineFactory factory = this.eng.getFactory();

        assertNotNull(factory);

        List<String> names = factory.getNames();

        assertTrue(names.contains("jav8"));
        assertTrue(names.contains("javascript"));

        List<String> mimeTypes = factory.getMimeTypes();

        assertTrue(mimeTypes.contains("text/javascript"));

        List<String> extensions = factory.getExtensions();

        assertTrue(extensions.contains("js"));
    }

    @Test
    public void testEvalStringScriptContext()
    {
        fail("Not yet implemented");
    }

    @Test
    public void testCreateBindings()
    {
        fail("Not yet implemented");
    }

    @Test
    public void testGetFactory()
    {
        fail("Not yet implemented");
    }

}
