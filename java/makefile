clio:
	@mkdir -p bin
	javac -d bin src/ArgParser.java

example:
	@make clio
	javac -d bin -cp bin src/Example.java
	java -cp bin Example

tests:
	@make clio
	javac -d bin -cp bin:lib/junit-4.12.jar src/ArgParserTest.java
	java -cp bin:lib/junit-4.12.jar:lib/hamcrest-core-1.3.jar \
	  org.junit.runner.JUnitCore ArgParserTest

test:
	@make tests

clean:
	rm bin/*.class
