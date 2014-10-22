// Import the DSS packages into our namespace to save on typing
importPackage(Packages.com.ti.debug.engine.scripting)
importPackage(Packages.com.ti.ccstudio.scripting.environment)
importPackage(Packages.java.lang)

function printTrace(string)
{
    script.traceWrite(string);
}

// Configurable Parameters
var deviceCCXMLFile = "./Aalto2_OBC.ccxml";
var programToLoad = "../Debug/Aalto2_FreeRTOS.out";

// Create our scripting environment object - which is the main entry point into any script and
// the factory for creating other Scriptable ervers and Sessions
var script = ScriptingEnvironment.instance();

script.setScriptTimeout(-1);
script.traceBegin("Log.xml", "DefaultStylesheet.xsl");

// Create a debug server
var ds = script.getServer( "DebugServer.1" );

// Set the device ccxml 
ds.setConfig( deviceCCXMLFile );

// Open a debug session
debugSession = ds.openSession("*", "*");

printTrace("TARGET: " + debugSession.getBoardName());
debugSession.target.connect();
printTrace("Connected");

// Load the program 
debugSession.memory.loadProgram( programToLoad );
printTrace("Load complete");

