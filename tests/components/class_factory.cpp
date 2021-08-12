#include <catch.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <stlext/components/class_factory.hpp>


#ifdef STDX_OS_LINUX
#include <cxxabi.h>
#endif

struct demangler
{
    template<class T>
    demangler() :
        __m_mangled(typeid(T).name()) {
    }

    demangler(const std::type_info& ti) :
        __m_mangled(ti.name()) {
    }

    demangler(const char* symbol) :
        __m_mangled(symbol) {
    }


    operator std::string() const {
        std::string buf(__m_mangled);
#ifdef STDX_OS_LINUX
        demangle(buf);
#endif
        return buf;
    }

    int demangle(std::string& result) const {
#ifdef STDX_OS_LINUX
        int status = 0;
        char *demangled = abi::__cxa_demangle(__m_mangled, 0, 0, &status);
        if (demangled != nullptr) {
            result = demangled;
            free(demangled);
        }
        return status;
#else
        result = __m_mangled;
        return 0;
#endif
    }

    template<class _OStream>
    friend _OStream& operator<< (_OStream& out, const demangler& d) {
#ifdef STDX_OS_LINUX
        int status = 0;
        char *demangled = abi::__cxa_demangle(d.__m_mangled, 0, 0, &status);
        if (demangled != nullptr) {
            out << (status == 0 ? demangled : d.__m_mangled);
            free(demangled);
        } else {
            out << d.__m_mangled;
        }
#else
        out << d.__m_mangled;
#endif
        return (out);
    }
private:
    const char* __m_mangled;
};

inline demangler demangled(const char* symbol) {
    return demangler(symbol);
}

inline demangler demangled(const std::type_info& ti) {
    return demangler(ti);
}

template<class T>
inline demangler demangled() {
    return demangler(typeid(T));
}







class shape
{
public:
	typedef std::shared_ptr<shape> pointer;
	shape(shape* _parent = 0, int _id = 0) :
        mParent(_parent), mId(_id) {
	}

	virtual ~shape() {}

	virtual void draw(std::ostream&) const = 0;

	int id() const { return mId; }
	shape* parent() const { return mParent; }
private:
	shape* mParent;
    int mId;
};

class rect : public shape
{
public:
	rect(shape* _parent = 0, int _id = 0) : shape(_parent, _id) {}
	void draw(std::ostream& out) const { out << "I'm rect" << std::endl; }
};

class ellipse : public shape
{
public:
	ellipse(shape* _parent = 0, int _id = 0) : shape(_parent, _id) {}
	void draw(std::ostream& out) const { out << "I'm ellipse" << std::endl; }
};

class line : public shape
{
public:
	line(shape* _parent = 0, int _id = 0) : shape(_parent, _id) {}
	void draw(std::ostream& out) const { out << "I'm line" << std::endl; }
};



TEST_CASE("class_factory", "[components]")
{
	using namespace std;

	typedef stdx::class_factory<shape, string> shape_factory;

	shape_factory factory;
	factory.registrate<rect>("com.shapes.rect");
	factory.registrate<rect, shape*>("com.shapes.rect");
	factory.registrate<rect(shape*, int)>("com.shapes.rect");

	factory.registrate<ellipse()>("com.shapes.ellipse");
	factory.registrate<ellipse(shape*)>("com.shapes.ellipse");
	factory.registrate<ellipse, shape*, int>("com.shapes.ellipse");

	factory.registrate<line()>("com.shapes.line");
	factory.registrate<line(shape*)>("com.shapes.line");
	factory.registrate<line(shape*, int)>("com.shapes.line");

    //cout << "shape factory: [" << factory.typeinfo()->name() << ']' << endl;
    cout << "shape factory: [" << demangled<shape_factory>() << ']' << endl;
    cout << "shape factory base: [" << demangled(*factory.typeinfo()) << ']' << endl;
	for (auto it = factory.begin(); it != factory.end(); ++it) {
        //cout << *it << " [" << it.product_type().name() << "] " << it.args_type().name() << endl;
        cout << it->first << " [" << demangled(it.product_type()) << "] " << demangled(it->second) << endl;
	}

    //REQUIRE(typeid(rect).name()    == factory.typeinfo("com.shapes.rect")->name());
    //REQUIRE(typeid(ellipse).name() == factory.typeinfo("com.shapes.ellipse")->name());
    //REQUIRE(typeid(line).name()    == factory.typeinfo("com.shapes.line")->name());

	vector<std::unique_ptr<shape>> shapes; 
	
	shapes.emplace_back(factory.create_object("com.shapes.rect"));
	shapes.emplace_back(factory.create_object("com.shapes.ellipse", shapes.back().get()));
	shapes.emplace_back(factory.create_object("com.shapes.line", shapes.back().get(), 121));

	for (auto it = shapes.begin(); it != shapes.end(); ++it)
		REQUIRE(*it != nullptr);
}
