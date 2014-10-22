var interrupt = debugSession.symbol.getAddress("i2cInterrupt");
var bp1 = debugSession.breakpoint.add(interrupt);
debugSession.target.run();
printTrace("interrupt!");
printTrace(debugSession.memory.readRegister("CPSR"));

debugSession.target.run();
printTrace("interrupt!");

debugSession.target.run();
printTrace("interrupt!");
