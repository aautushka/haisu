#include "benchmark/benchmark_api.h"
#include "haisu/zset.h"
#include <set>
#include <algorithm>


template <typename T>
class sorted_vec
{
public:
	sorted_vec(std::initializer_list<T> ll)
	{
		for (auto l: ll)
		{
			vec.push_back(l);
		}
		std::sort(vec.begin(), vec.end());
	}

	size_t count(const T& needle) const
	{
		return std::binary_search(vec.begin(), vec.end(), needle) ? 1 : 0;
	}
	
private:
	std::vector<T> vec;
};

static void bench_zset_failed_lookup_small_dataset(benchmark::State& state) 
{
	haisu::zset z = {"a", "b", "big", "bdi", "cite", "em", "font", "i", "img", "mark", "small", "span", "strike", "strong", "sub", "sup", "u"};

	while (state.KeepRunning())
	{
		for (int i = 0; i < 1000; ++i)
			volatile auto a = z.count("abracadabra");
	}
}

static void bench_set_failed_lookup_small_dataset(benchmark::State& state) 
{
	std::set<std::string> z = {"a", "b", "big", "bdi", "cite", "em", "font", "i", "img", "mark", "small", "span", "strike", "strong", "sub", "sup", "u"};
	std::string needle("abracadabra");
	
	while (state.KeepRunning())
	{
		for (int i = 0; i < 1000; ++i)
			volatile auto a = z.count(needle);
	}
}

static void bench_sorted_vec_failed_lookup_small_dataset(benchmark::State& state) 
{
	sorted_vec<std::string> z = {"a", "b", "big", "bdi", "cite", "em", "font", "i", "img", "mark", "small", "span", "strike", "strong", "sub", "sup", "u"};
	std::string needle("abracadabra");
	
	while (state.KeepRunning())
	{
		for (int i = 0; i < 1000; ++i)
			volatile auto a = z.count(needle);
	}
}

static void bench_zset_failed_lookup_large_dataset(benchmark::State& state) 
{
	haisu::zset z = {
		"a", "b", "big", "bdi", "cite", "em", "font", "i", "img", "mark", "small", "span", "strike", "strong", "sub", "sup", "u", 
		"img", "applet", "embed", "object", "font", "basefont", "br", "script", "map", "q", "sub", "sup", "span", "bdo", "iframe",
		"pre", "p", "dl", "div", "center", "noscript", "noframes", "blockquote", "form", "isindex", "hr", "table", "fieldset", "address",
		"input", "select", "textarea", "label", "button", "h1", "h2", "h3", "h4", "h5", "h6",
		"onclick", "ondblclick", "onmousedown", "onmouseup", "onmouseover", "onmouseout", "onkeypress", "onkeydown", "onkeyup",
		"charset", "type", "name", "href", "hreflang", "rel", "rev", "accesskey", "shape", "coords", "tabindex", "onfocus", "onblur",
		"longdesc", "name", "src", "frameborder", "marginwidth", "marginheight", "noresize", "scrolling",
		"title", "isindex", "base", "script", "style", "meta", "link", "object",
		"longdesc", "name", "src", "frameborder", "marginwidth", "marginheight", "scrolling", "align", "height", "width",
		"longdesc", "name", "height", "width", "usemap", "ismap", "align", "alt", "border", "code", "codebase", "frameborder", "height", "hidden", "hspace", "name", "palette", "pluginspace", "pluginurl", "src", "type", "units", "vspace", "width",
		"type", "name", "value", "checked", "disabled", "readonly", "size", "maxlength", "src", "alt", "usemap", "ismap", "tabindex", "accesskey", "onfocus", "onblur", "onselect", "onchange", "accept"
	};
	
	while (state.KeepRunning())
	{
		for (int i = 0; i < 1000; ++i)
			volatile auto a = z.count("abracadabra");
	}
}

static void bench_set_failed_lookup_large_dataset(benchmark::State& state) 
{
	std::set<std::string> z = {
		"a", "b", "big", "bdi", "cite", "em", "font", "i", "img", "mark", "small", "span", "strike", "strong", "sub", "sup", "u", 
		"img", "applet", "embed", "object", "font", "basefont", "br", "script", "map", "q", "sub", "sup", "span", "bdo", "iframe",
		"pre", "p", "dl", "div", "center", "noscript", "noframes", "blockquote", "form", "isindex", "hr", "table", "fieldset", "address",
		"input", "select", "textarea", "label", "button", "h1", "h2", "h3", "h4", "h5", "h6",
		"onclick", "ondblclick", "onmousedown", "onmouseup", "onmouseover", "onmouseout", "onkeypress", "onkeydown", "onkeyup",
		"charset", "type", "name", "href", "hreflang", "rel", "rev", "accesskey", "shape", "coords", "tabindex", "onfocus", "onblur",
		"longdesc", "name", "src", "frameborder", "marginwidth", "marginheight", "noresize", "scrolling",
		"title", "isindex", "base", "script", "style", "meta", "link", "object",
		"longdesc", "name", "src", "frameborder", "marginwidth", "marginheight", "scrolling", "align", "height", "width",
		"longdesc", "name", "height", "width", "usemap", "ismap", "align", "alt", "border", "code", "codebase", "frameborder", "height", "hidden", "hspace", "name", "palette", "pluginspace", "pluginurl", "src", "type", "units", "vspace", "width",
		"type", "name", "value", "checked", "disabled", "readonly", "size", "maxlength", "src", "alt", "usemap", "ismap", "tabindex", "accesskey", "onfocus", "onblur", "onselect", "onchange", "accept"
	};

	std::string needle("abracadabra");
	
	while (state.KeepRunning())
	{
		for (int i = 0; i < 1000; ++i)
			volatile auto a  = z.count(needle);
	}
}


BENCHMARK(bench_zset_failed_lookup_small_dataset);
BENCHMARK(bench_set_failed_lookup_small_dataset);
BENCHMARK(bench_zset_failed_lookup_large_dataset);
BENCHMARK(bench_set_failed_lookup_large_dataset);
BENCHMARK(bench_sorted_vec_failed_lookup_small_dataset);

BENCHMARK_MAIN();
