
compile:
	@mkdir -p bin
	javac -d bin -cp lib/junit-4.12.jar src/*.java

example:
	@java -cp bin Example

test:
	@java -cp bin:lib/junit-4.12.jar:lib/hamcrest-core-1.3.jar org.junit.runner.JUnitCore ArgParserTest

clean:
	rm bin/*.class
