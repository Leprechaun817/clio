clio:
	@mkdir -p bin
	javac -d bin src/ArgParser.java

example:
	@make clio
	javac -d bin -cp bin src/Example.java
	java -cp bin Example

unittests:
	@make clio
	javac -d bin -cp bin:lib/junit-4.12.jar src/ArgParserTest.java

check:
	@make unittests
	java -cp bin:lib/junit-4.12.jar:lib/hamcrest-core-1.3.jar \
	  org.junit.runner.JUnitCore ArgParserTest

clean:
	rm bin/*.class
