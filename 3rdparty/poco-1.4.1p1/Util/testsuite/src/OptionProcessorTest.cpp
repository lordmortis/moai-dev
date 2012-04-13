//
// OptionProcessorTest.cpp
//
// $Id: //poco/1.4/Util/testsuite/src/OptionProcessorTest.cpp#1 $
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OptionProcessorTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/OptionProcessor.h"
#include "Poco/Util/OptionException.h"


using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionProcessor;


OptionProcessorTest::OptionProcessorTest(const std::string& name): CppUnit::TestCase(name)
{
}


OptionProcessorTest::~OptionProcessorTest()
{
}


void OptionProcessorTest::testUnix()
{
	OptionSet set;
	set.addOption(
		Option("include-dir", "I", "specify a search path for locating header files")
			.required(false)
			.repeatable(true)
			.argument("path"));
			
	set.addOption(
		Option("library-dir", "L", "specify a search path for locating library files")
			.required(false)
			.repeatable(true)
			.argument("path"));

	set.addOption(
		Option("output", "o", "specify the output file", true)
			.argument("file", true));

	set.addOption(
		Option("verbose", "v")
		.description("enable verbose mode")
		.required(false)
		.repeatable(false));
		
	set.addOption(
		Option("optimize", "O")
		.description("enable optimization")
		.required(false)
		.repeatable(false)
		.argument("level", false)
		.group("mode"));
		
	set.addOption(
		Option("debug", "g")
		.description("generate debug information")
		.required(false)
		.repeatable(false)
		.group("mode"));

	set.addOption(
		Option("info", "i")
		.description("print information")
		.required(false)
		.repeatable(false));

	OptionProcessor p1(set);
	std::string name;
	std::string value;
	
	assert (p1.process("-I/usr/include", name, value));
	assert (name == "include-dir");
	assert (value == "/usr/include");

	assert (p1.process("--include:/usr/local/include", name, value));
	assert (name == "include-dir");
	assert (value == "/usr/local/include");

	assert (p1.process("--lib=/usr/local/lib", name, value));
	assert (name == "library-dir");
	assert (value == "/usr/local/lib");
	
	assert (p1.process("-ofile", name, value));
	assert (name == "output");
	assert (value == "file");
	
	assert (!p1.process("src/file.cpp", name, value));
	assert (!p1.process("/src/file.cpp", name, value));
	
	try
	{
		p1.process("--output:file", name, value);
		fail("duplicate - must throw");
	}
	catch (Poco::Util::DuplicateOptionException&)
	{
	}
	
	assert (p1.process("-g", name, value));
	assert (name == "debug");
	assert (value == "");
	
	try
	{
		p1.process("--optimize", name, value);
		fail("incompatible - must throw");
	}
	catch (Poco::Util::IncompatibleOptionsException&)
	{
	}
	
	try
	{
		p1.process("-x", name, value);
		fail("unknown option - must throw");
	}
	catch (Poco::Util::UnknownOptionException&)
	{
	}

	try
	{
		p1.process("--in", name, value);
		fail("ambiguous option - must throw");
	}
	catch (Poco::Util::AmbiguousOptionException&)
	{
	}
}


void OptionProcessorTest::testDefault()
{
	OptionSet set;
	set.addOption(
		Option("include-dir", "I", "specify a search path for locating header files")
			.required(false)
			.repeatable(true)
			.argument("path"));
			
	set.addOption(
		Option("library-dir", "L", "specify a search path for locating library files")
			.required(false)
			.repeatable(true)
			.argument("path"));

	set.addOption(
		Option("output", "o", "specify the output file", true)
			.argument("file", true));

	set.addOption(
		Option("verbose", "v")
		.description("enable verbose mode")
		.required(false)
		.repeatable(false));
		
	set.addOption(
		Option("optimize", "O")
		.description("enable optimization")
		.required(false)
		.repeatable(false)
		.argument("level", false)
		.group("mode"));
		
	set.addOption(
		Option("debug", "g")
		.description("generate debug information")
		.required(false)
		.repeatable(false)
		.group("mode"));

	set.addOption(
		Option("info", "i")
		.description("print information")
		.required(false)
		.repeatable(false));

	OptionProcessor p1(set);
	p1.setUnixStyle(false);
	std::string name;
	std::string value;
	
	assert (p1.process("/Inc:/usr/include", name, value));
	assert (name == "include-dir");
	assert (value == "/usr/include");

	assert (p1.process("/include:/usr/local/include", name, value));
	assert (name == "include-dir");
	assert (value == "/usr/local/include");

	assert (p1.process("/lib=/usr/local/lib", name, value));
	assert (name == "library-dir");
	assert (value == "/usr/local/lib");
	
	assert (p1.process("/out:file", name, value));
	assert (name == "output");
	assert (value == "file");
	
	assert (!p1.process("src/file.cpp", name, value));
	assert (!p1.process("\\src\\file.cpp", name, value));
	
	try
	{
		p1.process("/output:file", name, value);
		fail("duplicate - must throw");
	}
	catch (Poco::Util::DuplicateOptionException&)
	{
	}
	
	assert (p1.process("/debug", name, value));
	assert (name == "debug");
	assert (value == "");
	
	try
	{
		p1.process("/OPT", name, value);
		fail("incompatible - must throw");
	}
	catch (Poco::Util::IncompatibleOptionsException&)
	{
	}
	
	try
	{
		p1.process("/x", name, value);
		fail("unknown option - must throw");
	}
	catch (Poco::Util::UnknownOptionException&)
	{
	}

	try
	{
		p1.process("/in", name, value);
		fail("ambiguous option - must throw");
	}
	catch (Poco::Util::AmbiguousOptionException&)
	{
	}
}


void OptionProcessorTest::testRequired()
{
	OptionSet set;
	set.addOption(
		Option("option", "o")
			.required(true)
			.repeatable(true));

	OptionProcessor p1(set);
	std::string name;
	std::string value;
	
	try
	{
		p1.checkRequired();
		fail("no options specified - must throw");
	}
	catch (Poco::Util::MissingOptionException&)
	{
	}
	
	assert (p1.process("-o", name, value));
	p1.checkRequired();
}


void OptionProcessorTest::setUp()
{
}


void OptionProcessorTest::tearDown()
{
}


CppUnit::Test* OptionProcessorTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("OptionProcessorTest");

	CppUnit_addTest(pSuite, OptionProcessorTest, testUnix);
	CppUnit_addTest(pSuite, OptionProcessorTest, testDefault);
	CppUnit_addTest(pSuite, OptionProcessorTest, testRequired);

	return pSuite;
}