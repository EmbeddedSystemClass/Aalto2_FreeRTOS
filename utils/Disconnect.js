// All done
debugSession.terminate()
ds.stop()

script.traceSetConsoleLevel(TraceLevel.INFO)

// Stop logging and exit.
script.traceEnd();
printTrace("Disocnnected");
