#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <regex>

// std::wstring版本
std::vector<std::wstring> splitPath1(const std::wstring& in)
{
	std::wstring delim = L"[\\\\/]+"; // Regular expression to match one or more '\' or '/' characters
	std::wregex re{ delim };
	return std::vector<std::wstring> {
		std::wsregex_token_iterator(in.begin(), in.end(), re, -1),
			std::wsregex_token_iterator()
	};
}

// 用于路径分割的函数，可以处理不同的分隔符
std::vector<std::wstring> splitPath(const std::wstring& path) {
	std::vector<std::wstring> parts;
	std::wstring temp;
	for (auto ch : path) {
		if (ch == L'\\' || ch == L'/') {
			if (!temp.empty()) {
				parts.push_back(temp);
				temp.clear();
			}
		}
		else {
			temp += ch;
		}
	}
	if (!temp.empty()) {
		parts.push_back(temp);
	}
	return parts;
}


// 高效的字符串匹配算法
std::wstring findBestMatch(
	const std::multimap<std::wstring, std::wstring>& fileMap,
	const std::wstring& wm_strFileName,
	const std::wstring& wCurLinkPath)
{
	auto range = fileMap.equal_range(wm_strFileName);
	std::vector<std::pair<std::wstring, std::wstring>> candidates(range.first, range.second);

	if (candidates.empty()) {
		return L""; // 未找到匹配项
	}

	if (candidates.size() == 1) {
		return candidates.front().second; // 只有一个匹配项，直接返回
	}

	// 拆分 wCurLinkPath 成路径片段
	std::vector<std::wstring> linkParts = splitPath(wCurLinkPath);

	// 查找最佳匹配路径
	std::wstring bestMatch;
	int maxMatchCount = 0;

	for (const auto& candidate : candidates) {
		std::vector<std::wstring> candidateParts = splitPath(candidate.second);
		int matchCount = 0;

		// 从后向前匹配路径片段
		auto itLink = linkParts.rbegin();
		auto itCandidate = candidateParts.rbegin();
		while (itLink != linkParts.rend() && itCandidate != candidateParts.rend()) {
			if (*itLink == *itCandidate) {
				matchCount++;
			}
			else {
				break;
			}
			++itLink;
			++itCandidate;
		}

		if (matchCount > maxMatchCount) {
			maxMatchCount = matchCount;
			bestMatch = candidate.second;
		}
	}

	return bestMatch;
}

int main() {
	std::multimap<std::wstring, std::wstring> fileMap = {
		{L"a.rvt", L"C:\\1\\3\\res\\6\\7//a.rvt"},
		//{L"a.rvt", L"C:\\1\\3\\res\\6\\a.rvt"}
	};
	//fileMap.insert({ L"a.rvt", L"C:\\1\\3\\res/a.rvt" });
	//fileMap.insert({ L"a.rvt", L"C:/1/3/res\a.rvt" } );//存在就是\a，不可能有单独的\
	
	fileMap.insert({ L"a.rvt", L"C:\\1\\/3\\\\res\\//a.rvt" });
	fileMap.insert({ L"a.rvt", L"C:\\\\\\1\/3\\/res\\\\a.rvt" });
	
	std::wstring wm_strFileName = L"a.rvt";
	std::wstring wCurLinkPath = L"res\\a.rvt";


	std::wstring result = findBestMatch(fileMap, wm_strFileName, wCurLinkPath);
	std::wcout << L"Best match: " << result << std::endl;

	return 0;
}