#include <gtest/gtest.h>
#include <cpp_reflection/cpp_reflection.hpp>

class reflectable_class {
private:
    unsigned int nb;

public:
    //! ctor
    reflectable_class(void) : nb(42) {}

public:
    unsigned int get_42(void) { return 42; }
    unsigned int get_nb(void) { return nb; }
    void set_nb(unsigned int nb) { this->nb = nb; }

    void do_something(const std::string&, int& nb, char, float, int nb2) { nb = nb2; }

    int add(int nb1, int nb2) { return nb1 + nb2; }

    static void static_fct(void) {}
};

REGISTER_CLASS_FUNCTIONS(reflectable_class, (get_42)(do_something)(add)(get_nb)(set_nb)(static_fct))

TEST(MemberFunctionReflectionWithInstance, ReturnGoodValue) {
    reflectable_class obj;
    EXPECT_EQ(42, cpp_reflection::reflection_maker<unsigned int()>::invoke(&obj, "reflectable_class", "get_42"));
}

TEST(MemberFunctionReflectionWithInstance, AcceptMultipleParams) {
    int nb = 0;

    reflectable_class obj;
    cpp_reflection::reflection_maker<void(const std::string&, int&, char, float, int)>::invoke(&obj, "reflectable_class", "do_something",
                                                                                               std::string("hello"), nb, 'a', 1.0, 42);

    EXPECT_EQ(42, nb);
}

TEST(MemberFunctionReflectionWithInstance, UpdateObject) {
    reflectable_class obj;

    int nb = cpp_reflection::reflection_maker<unsigned int()>::invoke(&obj, "reflectable_class", "get_nb");
    cpp_reflection::reflection_maker<void(unsigned int)>::invoke(&obj, "reflectable_class", "set_nb", nb * 2);

    EXPECT_EQ(2 * nb, cpp_reflection::reflection_maker<unsigned int()>::invoke(&obj, "reflectable_class", "get_nb"));
}

TEST(MemberFunctionReflectionWithInstance, DoExpectedJob) {
    reflectable_class obj;
    EXPECT_EQ(42, cpp_reflection::reflection_maker<int(int, int)>::invoke(&obj, "reflectable_class", "add", 20, 22));
}

TEST(MemberFunctionReflectionWithInstance, UnregisteredFunction) {
    try {
        reflectable_class obj;
        cpp_reflection::reflection_maker<void()>::invoke(&obj, "reflectable_class", "unregistered");
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
    catch (const cpp_reflection::reflection_exception& e) {
        EXPECT_EQ(e.what(), std::string("Function reflectable_class::unregistered is not registered"));
    }
    catch (...) {
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
}

TEST(MemberFunctionReflectionWithInstance, StaticMemberFunction) {
    try {
        reflectable_class obj;
        cpp_reflection::reflection_maker<void()>::invoke(&obj, "reflectable_class", "static_fct");
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
    catch (const cpp_reflection::reflection_exception& e) {
        EXPECT_EQ(e.what(), std::string("Function reflectable_class::static_fct can't be called with object"));
    }
    catch (...) {
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
}

TEST(MemberFunctionReflectionWithInstance, UnregisteredClass) {
    try {
        reflectable_class obj;
        cpp_reflection::reflection_maker<void()>::invoke(&obj, "unregistered_class", "unregistered");
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
    catch (const cpp_reflection::reflection_exception& e) {
        EXPECT_EQ(e.what(), std::string("Class unregistered_class is not registered"));
    }
    catch (...) {
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
}


TEST(MemberFunctionReflectionWithInstance, WrongFunctionPrototype) {
    try {
        reflectable_class obj;
        cpp_reflection::reflection_maker<void()>::invoke(&obj, "reflectable_class", "get_nb");
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
    catch (const cpp_reflection::reflection_exception& e) {
        EXPECT_EQ(e.what(), std::string("Invalid function signature for function reflectable_class::get_nb"));
    }
    catch (...) {
        FAIL() << "Expected cpp_reflection::reflection_exception";
    }
}
