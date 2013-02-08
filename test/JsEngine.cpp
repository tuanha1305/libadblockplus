#include <FileReader.h>
#include <fstream>
#include <gtest/gtest.h>
#include <JsEngine.h>
#include <JsError.h>
#include <sstream>

struct StubFileReader : public AdblockPlus::FileReader
{
  std::auto_ptr<std::istream> Read(const std::string& path) const
  {
    std::stringstream* source = new std::stringstream;
    *source << "function hello() { return 'Hello'; }";
    return std::auto_ptr<std::istream>(source);
  }
};

struct BadFileReader : public AdblockPlus::FileReader
{
  std::auto_ptr<std::istream> Read(const std::string& path) const
  {
    std::ifstream* file = new std::ifstream;
    file->open("");
    return std::auto_ptr<std::istream>(file);
  }
};

TEST(JsEngineTest, EvaluateAndCall)
{
  AdblockPlus::JsEngine jsEngine;
  const std::string source = "function hello() { return 'Hello'; }";
  jsEngine.Evaluate(source);
  const std::string result = jsEngine.Call("hello");
  ASSERT_EQ("Hello", result);
}

TEST(JsEngineTest, LoadAndCall)
{
  StubFileReader fileReader;
  AdblockPlus::JsEngine jsEngine;
  jsEngine.fileReader = &fileReader;
  jsEngine.Load("hello.js");
  const std::string result = jsEngine.Call("hello");
  ASSERT_EQ("Hello", result);
}

TEST(JsEngineTest, LoadBadStreamFails)
{
  BadFileReader fileReader;
  AdblockPlus::JsEngine jsEngine;
  jsEngine.fileReader = &fileReader;
  ASSERT_ANY_THROW(jsEngine.Load("hello.js"));
}

TEST(JsEngineTest, JsExceptionIsThrown)
{
  AdblockPlus::JsEngine jsEngine;
  ASSERT_THROW(jsEngine.Evaluate("doesnotexist()"), AdblockPlus::JsError);
}
