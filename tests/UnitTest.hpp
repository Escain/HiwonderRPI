/*
 * This file is part of HiwonderRPI library
 * 
 * HiwonderRPI is free software: you can redistribute it and/or modify 
 * it under ther terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * HiwonderRPI is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with HiwonderRPI. If not, see <https://www.gnu.org/licenses/>.
 * 
 * Author: Adrian Maire escain (at) gmail.com
 */

#include <iostream>
#include <string>
#include <functional>
#include <list>

/*
 * Create tests in the following way:
 * UNIT_TEST(NameOfTheTest)
 * {
 *     ASSERT(myFunction()==expectedResult);
 * }
 *
 * Create unit_tests with a base clase like
 * UNIT_TEST(NameOfTheTest, BaseClass)
 * ...
 *
 * You may use the constructor to setup element, and use
 * Base member in your tests.
 */

//*-----------------------------------------------------------------------
/// Base class for all unit-test
class TEST
{
	bool m_result=true; ///save the result of the test: true==pass, false==Fail
	char _unused[7];       /// Avoid alignment warning
public:
	virtual std::string getName() const =0; /// Return the textual name of the test
	virtual ~TEST() = default;
	virtual void run()=0;                   /// Run the test

	void setFail(){m_result=false;}         /// Save the result of the test Pass/true, Fail/false
	bool isPassed(){return m_result;}       /// Retrieve the result of the test
};

//*------------------------------------------------------------------------
/// Registry to save all classes
class Registry
{
public:
	using factoryMethod = std::function<TEST*()>;

    static void registerClass(const std::string& name, factoryMethod meth)
    {
		registry.push_back(std::make_pair(name, meth));
    }
	static const std::list<std::pair<std::string, factoryMethod>>& getRegistry() { return registry; }

private:
	static std::list<std::pair<std::string, factoryMethod>> registry;
};

std::list<std::pair<std::string, Registry::factoryMethod>> Registry::registry;


//*-------------------------------------------------------------------------
/// ASSERT MACROS
#define ASSERT(cond)                                         \
	if( !(cond) )                                            \
	{                                                        \
	    std::cout <<  "    " << __LINE__ << " FAIL ASSERT: " \
	        << getName() << " : " << #cond << std::endl;     \
	    setFail();                                           \
	    return;                                              \
	}

#define ASSERT_EQ(a,b)                                       \
	if(a!=b)                                                 \
	{                                                        \
	    std::cout << "    " << __LINE__ << " FAIL EQ: "      \
	        << getName() << " : " << #a << "("<< a <<        \
	        ") != "<<#b <<"(" <<b<<")" << std::endl;         \
	    setFail();                                           \
	    return;                                              \
	}

#define ASSERT_NEQ(a,b)                                      \
	if(a!=b)                                                 \
	{                                                        \
	    std::cout << "    " << __LINE__ << " FAIL NEQ: "     \
	        << getName() << " : " << #a << "("<< a <<        \
	        ") == "<<#b <<"(" <<b<<")" << std::endl;         \
	    setFail();                                           \
	    return;                                              \
	}


//*-------------------------------------------------------------------------
#define REGISTER_CLASS(ClassType)                            \
	static TEST* createInstance()                            \
	{                                                        \
	    return new ClassType();                              \
	}                                                        \
	static int initRegistry()                                \
	{                                                        \
	    Registry::registerClass(                             \
	        std::string(#ClassType),                         \
	        ClassType::createInstance);                      \
	    return 0;                                            \
	}                                                        \
	static const int regToken;                               \


#define DEFINE_REG_CLASS(ClassType)                          \
	const int ClassType::regToken = ClassType::initRegistry();

/// UNIT_TEST standalone, without base class
#define UNIT_TEST1(name)                                     \
	struct name: public TEST                                 \
	{                                                        \
		std::string getName() const override                 \
		{ return #name;}                                     \
		void run() override;                                 \
		REGISTER_CLASS(name)                                 \
	};                                                       \
	DEFINE_REG_CLASS(name)                                   \
    void name::run()

/// UNIT_TEST with base-class
#define UNIT_TEST2(name,Base)                                \
	struct name: public TEST, public Base                    \
	{                                                        \
		std::string getName() const override                 \
		{ return #name;}                                     \
		void run() override;                                 \
		REGISTER_CLASS(name)                                 \
	};                                                       \
	DEFINE_REG_CLASS(name)                                   \
	void name::run()


//*----------------------------------------------------------------------
// Make UNIT_TEST to be overloaded for one or two parameters
#define GET_MACRO(_1,_2,NAME,...)                            \
	NAME

#define UNIT_TEST(...)                                       \
	GET_MACRO(__VA_ARGS__, UNIT_TEST2, UNIT_TEST1, _1)(__VA_ARGS__)



//*----------------------------------------------------------------------
// Main function, calling automatically all UNIT_TEST
int main()
{
	size_t count = 0;
	size_t countPassed = 0;
	for(auto entry : Registry::getRegistry())
	{
		const auto& constructor = entry.second;
		auto* obj = constructor();

		std::cout << "    RUNNING: " << obj->getName() << std::endl;
		obj->run();
		if (!obj->isPassed())
		{
			std::cout << "FAILED: " << obj->getName() << std::endl;
		}
		countPassed += obj->isPassed()?1:0;
		count++;
	}

	if (count == countPassed)
	{
		std::cout << "ALL TEST PASSED: " << count << std::endl;
	}
	else
	{
		std::cout << "FAILED: " << countPassed << " passed of " << count << std::endl;
	}

	return 0;
}
