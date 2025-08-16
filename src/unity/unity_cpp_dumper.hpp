#pragma once
#include "UnityResolve.hpp"
#include "il2cpp-tabledefs.h"
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <chrono>

class UnityCppDumper
{
    struct GenericInfo
    {
        std::string baseName;
        int parameterCount = 0;
        std::vector<std::string> parameterNames;
        bool isGeneric = false;
    };

    struct CachedData
    {
        std::unordered_map<UnityResolve::Class*, int> classFlagsCache;
        std::unordered_map<UnityResolve::Field*, int> fieldFlagsCache;
        std::unordered_map<UnityResolve::Method*, uint32_t> methodFlagsCache;
        std::unordered_map<UnityResolve::Class*, bool> isEnumCache;
        std::unordered_map<UnityResolve::Class*, bool> isValueTypeCache;
        std::unordered_map<UnityResolve::Class*, bool> isAbstractCache;
        std::unordered_map<UnityResolve::Class*, bool> isInterfaceCache;
        std::unordered_map<UnityResolve::Class*, bool> isGenericClassCache;
        std::unordered_map<UnityResolve::Method*, bool> isGenericMethodCache;
        std::unordered_map<UnityResolve::Field*, bool> isLiteralFieldCache;
        std::unordered_map<UnityResolve::Class*, int32_t> classInstanceSizeCache;

        std::unordered_map<std::string, std::string> sanitizedNameCache;
        std::unordered_map<std::string, std::string> typeMappingCache;
        std::unordered_map<std::string, GenericInfo> genericInfoCache;
        std::unordered_map<UnityResolve::Class*, std::string> assemblyNameCache;
        std::unordered_map<UnityResolve::Class*, std::unordered_map<std::string, int>> classMethodOverloadsCache;
        std::unordered_map<UnityResolve::Field*, std::string> literalValueCache;
        std::unordered_map<std::string, UnityResolve::Class*> classLookupCache;

        std::regex invalidCharsRegex{R"([<>`,\s\[\]\.&\*\+\-\(\)\$])"};
        std::regex multiUnderscoreRegex{"_{2,}"};

        size_t totalClasses = 0;
        size_t totalFields = 0;
        size_t totalMethods = 0;
    };

    inline static CachedData cache;
    inline static std::unordered_map<UnityResolve::Class*, UnityResolve::Assembly*> classToAssemblyMap;

    inline static void* il2cpp_class_get_flags_ptr = nullptr;
    inline static void* il2cpp_field_get_flags_ptr = nullptr;
    inline static void* il2cpp_method_get_flags_ptr = nullptr;
    inline static void* il2cpp_class_is_enum_ptr = nullptr;
    inline static void* il2cpp_class_is_valuetype_ptr = nullptr;
    inline static void* il2cpp_class_is_abstract_ptr = nullptr;
    inline static void* il2cpp_class_is_interface_ptr = nullptr;
    inline static void* il2cpp_class_is_generic_ptr = nullptr;
    inline static void* il2cpp_method_is_generic_ptr = nullptr;
    inline static void* il2cpp_field_is_literal_ptr = nullptr;
    inline static void* il2cpp_class_instance_size_ptr = nullptr;

    inline static void* mono_domain_get_ptr = nullptr;
    inline static void* mono_class_get_flags_ptr = nullptr;
    inline static void* mono_field_get_flags_ptr = nullptr;
    inline static void* mono_method_get_flags_ptr = nullptr;
    inline static void* mono_class_is_enum_ptr = nullptr;
    inline static void* mono_class_is_valuetype_ptr = nullptr;
    inline static void* mono_class_is_generic_ptr = nullptr;
    inline static void* mono_unity_class_field_is_literal_ptr = nullptr;
    inline static void* unity_mono_method_is_generic_ptr = nullptr;
    inline static void* mono_class_instance_size_ptr = nullptr;


    static void initializeApiPointers()
    {
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            il2cpp_class_get_flags_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                        "il2cpp_class_get_flags");
            il2cpp_field_get_flags_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                        "il2cpp_field_get_flags");
            il2cpp_method_get_flags_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                         "il2cpp_method_get_flags");
            il2cpp_class_is_enum_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                      "il2cpp_class_is_enum");
            il2cpp_class_is_valuetype_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                           "il2cpp_class_is_valuetype");
            il2cpp_class_is_abstract_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                          "il2cpp_class_is_abstract");
            il2cpp_class_is_interface_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                           "il2cpp_class_is_interface");
            il2cpp_class_is_generic_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                         "il2cpp_class_is_generic");
            il2cpp_method_is_generic_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                          "il2cpp_method_is_generic");
            il2cpp_field_is_literal_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                         "il2cpp_field_is_literal");
            il2cpp_class_instance_size_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                            "il2cpp_class_instance_size");
        }
        else
        {
            mono_domain_get_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_), "mono_domain_get");
            mono_class_get_flags_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                      "mono_class_get_flags");
            mono_field_get_flags_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                      "mono_field_get_flags");
            mono_method_get_flags_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                       "mono_method_get_flags");
            mono_class_is_enum_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_), "mono_class_is_enum");
            mono_class_is_valuetype_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                         "mono_class_is_valuetype");
            mono_class_is_generic_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                       "mono_class_is_generic");
            mono_unity_class_field_is_literal_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                                   "mono_unity_class_field_is_literal");
            unity_mono_method_is_generic_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                              "unity_mono_method_is_generic");
            mono_class_instance_size_ptr = GetProcAddress(static_cast<HMODULE>(UnityResolve::hmodule_),
                                                          "mono_class_instance_size");
        }
    }

    static void buildClassAssemblyMapping()
    {
        classToAssemblyMap.clear();
        cache.classFlagsCache.clear();
        cache.fieldFlagsCache.clear();
        cache.methodFlagsCache.clear();
        cache.isEnumCache.clear();
        cache.isValueTypeCache.clear();
        cache.isAbstractCache.clear();
        cache.isInterfaceCache.clear();
        cache.isGenericClassCache.clear();
        cache.isGenericMethodCache.clear();
        cache.isLiteralFieldCache.clear();
        cache.classInstanceSizeCache.clear();
        cache.sanitizedNameCache.clear();
        cache.typeMappingCache.clear();
        cache.genericInfoCache.clear();
        cache.assemblyNameCache.clear();
        cache.classMethodOverloadsCache.clear();
        cache.literalValueCache.clear();
        cache.classLookupCache.clear();

        for (const auto& pAssembly : UnityResolve::assembly)
        {
            for (const auto& pClass : pAssembly->classes)
            {
                classToAssemblyMap[pClass] = pAssembly;
                cache.assemblyNameCache[pClass] = pAssembly->name;

                std::string shortName = pClass->name;
                std::string fullName = pClass->namespaze.empty()
                                           ? pClass->name
                                           : pClass->namespaze + "." + pClass->name;
                cache.classLookupCache[shortName] = pClass;
                cache.classLookupCache[fullName] = pClass;

                std::unordered_map<std::string, int> methodCounts;
                for (const auto& pMethod : pClass->methods)
                {
                    methodCounts[pMethod->name]++;
                }
                cache.classMethodOverloadsCache[pClass] = methodCounts;

                cache.totalClasses++;
                cache.totalFields += pClass->fields.size();
                cache.totalMethods += pClass->methods.size();
            }
        }

        LOG_INFO("Built cache for {} classes, {} fields, {} methods",
                 cache.totalClasses, cache.totalFields, cache.totalMethods);
    }

    static UnityResolve::Assembly* getAssemblyForClass(UnityResolve::Class* pClass)
    {
        auto it = classToAssemblyMap.find(pClass);
        return it != classToAssemblyMap.end() ? it->second : nullptr;
    }

    static std::string getAssemblyNameForClass(UnityResolve::Class* pClass)
    {
        auto it = cache.assemblyNameCache.find(pClass);
        return it != cache.assemblyNameCache.end() ? it->second : "Unknown";
    }

    static GenericInfo parseGenericType(const std::string& typeName)
    {
        auto it = cache.genericInfoCache.find(typeName);
        if (it != cache.genericInfoCache.end())
        {
            return it->second;
        }

        GenericInfo info;
        info.isGeneric = false;

        size_t pos = typeName.find('`');
        if (pos != std::string::npos)
        {
            info.isGeneric = true;
            info.baseName = typeName.substr(0, pos);

            std::string countStr = typeName.substr(pos + 1);
            try
            {
                info.parameterCount = std::stoi(countStr);

                // Generate generic parameter names
                for (int i = 0; i < info.parameterCount; ++i)
                {
                    info.parameterNames.push_back("T" + std::to_string(i));
                }
            }
            catch (...)
            {
                info.parameterCount = 1;
                info.parameterNames.push_back("T");
            }
        }
        else
        {
            info.baseName = typeName;
        }

        cache.genericInfoCache[typeName] = info;
        return info;
    }

    static std::string getUniqueMethodName(UnityResolve::Class* pClass, UnityResolve::Method* pMethod,
                                           const std::unordered_map<std::string, int>& currentCounts)
    {
        std::string baseName = sanitizeName(pMethod->name);

        auto classOverloads = cache.classMethodOverloadsCache.find(pClass);
        if (classOverloads != cache.classMethodOverloadsCache.end())
        {
            auto methodCount = classOverloads->second.find(pMethod->name);
            if (methodCount != classOverloads->second.end() && methodCount->second > 1)
            {
                auto currentCount = currentCounts.find(pMethod->name);
                if (currentCount != currentCounts.end() && currentCount->second > 0)
                {
                    return baseName + "_" + std::to_string(currentCount->second);
                }
            }
        }

        return baseName;
    }

    static std::string extractStringLiteral(UnityResolve::Field* pField)
    {
        if (!isLiteralField(pField)) return "";

        try
        {
            if (pField->type->name == "System.String")
            {
                if (pField->static_field)
                {
                    void* stringPtr = nullptr;
                    pField->GetStaticValue(&stringPtr);

                    if (stringPtr)
                    {
                        if (const auto monoString = static_cast<UnityResolve::UnityType::String*>(stringPtr))
                        {
                            std::ostringstream oss;
                            oss << "\"";

                            for (int i = 0; i < monoString->m_stringLength; ++i)
                            {
                                wchar_t c = monoString->m_firstChar[i];

                                if (c >= 32 && c <= 126 && c != '"' && c != '\\')
                                {
                                    oss << static_cast<char>(c);
                                }
                                else if (c == '"')
                                {
                                    oss << "\\\"";
                                }
                                else if (c == '\\')
                                {
                                    oss << "\\\\";
                                }
                                else if (c == '\n')
                                {
                                    oss << "\\n";
                                }
                                else if (c == '\r')
                                {
                                    oss << "\\r";
                                }
                                else if (c == '\t')
                                {
                                    oss << "\\t";
                                }
                                else
                                {
                                    // Convert to Unicode escape sequence
                                    oss << "\\u" << std::setfill('0') << std::setw(4)
                                        << std::hex << std::uppercase << static_cast<unsigned int>(c);
                                }
                            }

                            oss << "\"";
                            return oss.str();
                        }
                    }
                }

                // Try to extract from field offset if it contains a string pointer
                if (pField->offset > 0)
                {
                    // This might be a pointer to a string literal
                    return "\"" + pField->name + "_LITERAL\"";
                }
            }

            return "\"" + pField->name + "_VALUE\"";
        }
        catch (...)
        {
            return "\"\"";
        }
    }

    static std::string extractNumericLiteral(UnityResolve::Field* pField)
    {
        if (!isLiteralField(pField)) return "0";

        try
        {
            // Close your eyes or just collapse this lmao
            if (pField->static_field)
            {
                if (pField->type->name == "System.Int32")
                {
                    int32_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.UInt32")
                {
                    uint32_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.Int64")
                {
                    int64_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.UInt64")
                {
                    uint64_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.Int16")
                {
                    int16_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.UInt16")
                {
                    uint16_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.SByte")
                {
                    int8_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.Byte")
                {
                    uint8_t value = 0;
                    pField->GetStaticValue(&value);
                    return std::to_string(static_cast<unsigned int>(value));
                }
                if (pField->type->name == "System.Single")
                {
                    float value = 0.0f;
                    pField->GetStaticValue(&value);
                    return std::to_string(value) + "f";
                }
                if (pField->type->name == "System.Double")
                {
                    double value = 0.0;
                    pField->GetStaticValue(&value);
                    return std::to_string(value);
                }
                if (pField->type->name == "System.Boolean")
                {
                    bool value = false;
                    pField->GetStaticValue(&value);
                    return value ? "true" : "false";
                }
            }


            // Use field offset as literal value
            return std::to_string(pField->offset);
        }
        catch (...)
        {
            return "0";
        }
    }

    static std::string extractLiteralValue(UnityResolve::Field* pField)
    {
        if (!isLiteralField(pField)) return "";

        auto it = cache.literalValueCache.find(pField);
        if (it != cache.literalValueCache.end())
        {
            return it->second;
        }

        std::string result;

        // Handle string literals
        if (pField->type->name == "System.String")
        {
            result = extractStringLiteral(pField);
        }
        // Handle numeric and boolean literals
        else if (pField->type->name == "System.Int32" || pField->type->name == "System.UInt32" ||
            pField->type->name == "System.Int64" || pField->type->name == "System.UInt64" ||
            pField->type->name == "System.Int16" || pField->type->name == "System.UInt16" ||
            pField->type->name == "System.SByte" || pField->type->name == "System.Byte" ||
            pField->type->name == "System.Single" || pField->type->name == "System.Double" ||
            pField->type->name == "System.Boolean")
        {
            result = extractNumericLiteral(pField);
        }
        else if (pField->klass && isEnum(pField->klass))
        {
            result = extractEnumLiteral(pField);
        }
        else
        {
            // Use field offset as literal value
            result = std::to_string(pField->offset);
        }

        cache.literalValueCache[pField] = result;
        return result;
    }

    // Get literal value with proper type casting
    static std::string getLiteralValueWithType(UnityResolve::Field* pField)
    {
        if (!isLiteralField(pField)) return "";

        std::string value = extractLiteralValue(pField);
        std::string type = getTypeMapping(pField->type->name);

        // Handle special cases for numeric types 
        if (pField->type->name == "System.Single" && value.find("f") == std::string::npos)
        {
            value += "f";
        }
        else if (pField->type->name == "System.Double" && value.find(".") != std::string::npos)
        {
            // Ensure double precision
            if (value.find("f") != std::string::npos)
            {
                value = value.substr(0, value.find("f"));
            }
        }

        return value;
    }

    static std::string getTypeMapping(const std::string& unityType, bool forTemplateDecl = false)
    {
        std::string cacheKey = unityType + (forTemplateDecl ? "_template" : "");
        auto cacheIt = cache.typeMappingCache.find(cacheKey);
        if (cacheIt != cache.typeMappingCache.end())
        {
            return cacheIt->second;
        }

        GenericInfo genericInfo = parseGenericType(unityType);

        // TODO: improve this
        if (genericInfo.isGeneric)
        {
            std::string result;
            if (forTemplateDecl)
            {
                // For template declarations, use the generic parameter names
                result = sanitizeName(genericInfo.baseName) + "<";
                for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
                {
                    if (i > 0) result += ", ";
                    result += genericInfo.parameterNames[i];
                }
                result += ">";
            }
            else
            {
                // For usage, determine if it's a known generic type
                if (genericInfo.baseName == "System.Collections.Generic.List")
                {
                    result = "UnityResolve::UnityType::List<T>*";
                }
                else if (genericInfo.baseName == "System.Collections.Generic.Dictionary")
                {
                    result = "UnityResolve::UnityType::Dictionary<TKey, TValue>*";
                }
                else
                {
                    // Custom generic class
                    result = sanitizeName(genericInfo.baseName) + "*";
                }
            }

            cache.typeMappingCache[cacheKey] = result;
            return result;
        }

        static const std::unordered_map<std::string, std::string> typeMap = {
            {"System.Void", "void"},
            {"System.Boolean", "bool"},
            {"System.SByte", "int8_t"},
            {"System.Byte", "uint8_t"},
            {"System.Int16", "int16_t"},
            {"System.UInt16", "uint16_t"},
            {"System.Int32", "int32_t"},
            {"System.UInt32", "uint32_t"},
            {"System.Int64", "int64_t"},
            {"System.UInt64", "uint64_t"},
            {"System.Single", "float"},
            {"System.Double", "double"},
            {"System.IntPtr", "void*"},
            {"System.UIntPtr", "uintptr_t"},
            {"System.Char", "wchar_t"},
            // TODO: Probably define a common header for our own usage purposes
            {"System.String", "UnityResolve::UnityType::String*"},
            {"System.Object", "UnityResolve::UnityType::Object*"},
            {"UnityEngine.Vector2", "UnityResolve::UnityType::Vector2"},
            {"UnityEngine.Vector3", "UnityResolve::UnityType::Vector3"},
            {"UnityEngine.Vector4", "UnityResolve::UnityType::Vector4"},
            {"UnityEngine.Quaternion", "UnityResolve::UnityType::Quaternion"},
            {"UnityEngine.Matrix4x4", "UnityResolve::UnityType::Matrix4x4"},
            {"UnityEngine.Color", "UnityResolve::UnityType::Color"},
            {"UnityEngine.Rect", "UnityResolve::UnityType::Rect"},
            {"UnityEngine.GameObject", "UnityResolve::UnityType::GameObject*"},
            {"UnityEngine.Transform", "UnityResolve::UnityType::Transform*"},
            {"UnityEngine.Component", "UnityResolve::UnityType::Component*"},
            {"UnityEngine.MonoBehaviour", "UnityResolve::UnityType::MonoBehaviour*"},
            {"UnityEngine.Behaviour", "UnityResolve::UnityType::Behaviour*"},
            {"UnityEngine.Camera", "UnityResolve::UnityType::Camera*"},
            {"UnityEngine.Rigidbody", "UnityResolve::UnityType::Rigidbody*"},
            {"UnityEngine.Collider", "UnityResolve::UnityType::Collider*"},
            {"UnityEngine.Renderer", "UnityResolve::UnityType::Renderer*"},
            {"UnityEngine.Mesh", "UnityResolve::UnityType::Mesh*"},
            {"UnityEngine.Animator", "UnityResolve::UnityType::Animator*"}
        };

        auto it = typeMap.find(unityType);
        if (it != typeMap.end())
        {
            cache.typeMappingCache[cacheKey] = it->second;
            return it->second;
        }

        // Handle arrays
        if (unityType.find("[]") != std::string::npos)
        {
            std::string elementType = unityType.substr(0, unityType.find("[]"));
            std::string result = "UnityResolve::UnityType::Array<" + getTypeMapping(elementType) + ">*";
            cache.typeMappingCache[cacheKey] = result;
            return result;
        }

        // Handle pointers and references
        if (unityType.back() == '&')
        {
            std::string result = getTypeMapping(unityType.substr(0, unityType.length() - 1)) + "&";
            cache.typeMappingCache[cacheKey] = result;
            return result;
        }
        if (unityType.back() == '*')
        {
            std::string result = getTypeMapping(unityType.substr(0, unityType.length() - 1)) + "*";
            cache.typeMappingCache[cacheKey] = result;
            return result;
        }

        auto classIt = cache.classLookupCache.find(unityType);
        if (classIt != cache.classLookupCache.end())
        {
            UnityResolve::Class* pClass = classIt->second;
            std::string result;

            if (isEnum(pClass))
            {
                result = sanitizeName(pClass->name) + "_Enum";
            }
            else if (isValueType(pClass) && !isEnum(pClass))
            {
                result = sanitizeName(pClass->name);
            }
            else
            {
                result = sanitizeName(pClass->name) + "*";
            }

            cache.typeMappingCache[cacheKey] = result;
            return result;
        }

        // For reference types, use pointer
        std::string typeName = unityType;
        size_t lastDot = typeName.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            typeName = typeName.substr(lastDot + 1);
        }

        std::string result = sanitizeName(typeName) + "*";
        cache.typeMappingCache[cacheKey] = result;
        return result;
    }

    static int getClassFlags(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return 0;

        auto it = cache.classFlagsCache.find(pClass);
        if (it != cache.classFlagsCache.end())
        {
            return it->second;
        }

        int flags = 0;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_get_flags_ptr)
            {
                using il2cpp_class_get_flags_func = int(*)(void*);
                flags = static_cast<il2cpp_class_get_flags_func>(il2cpp_class_get_flags_ptr)(pClass->address);
            }
        }
        else
        {
            if (mono_class_get_flags_ptr)
            {
                using mono_class_get_flags_func = int(*)(void*);
                flags = static_cast<mono_class_get_flags_func>(mono_class_get_flags_ptr)(pClass->address);
            }
        }

        cache.classFlagsCache[pClass] = flags;
        return flags;
    }

    // Cached field flags lookup
    static int getFieldFlags(UnityResolve::Field* pField)
    {
        if (!pField || !pField->address) return 0;

        auto it = cache.fieldFlagsCache.find(pField);
        if (it != cache.fieldFlagsCache.end())
        {
            return it->second;
        }

        int flags = 0;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_field_get_flags_ptr)
            {
                using il2cpp_field_get_flags_func = int(*)(void*);
                flags = static_cast<il2cpp_field_get_flags_func>(il2cpp_field_get_flags_ptr)(pField->address);
            }
        }
        else
        {
            if (mono_field_get_flags_ptr)
            {
                using mono_field_get_flags_func = int(*)(void*);
                flags = static_cast<mono_field_get_flags_func>(mono_field_get_flags_ptr)(pField->address);
            }
        }

        cache.fieldFlagsCache[pField] = flags;
        return flags;
    }

    static uint32_t getMethodFlags(UnityResolve::Method* pMethod)
    {
        if (!pMethod || !pMethod->address) return 0;

        auto it = cache.methodFlagsCache.find(pMethod);
        if (it != cache.methodFlagsCache.end())
        {
            return it->second;
        }

        uint32_t flags = pMethod->flags;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_method_get_flags_ptr)
            {
                using il2cpp_method_get_flags_func = uint32_t(*)(void*, uint32_t*);
                uint32_t iflags = 0;
                flags = static_cast<il2cpp_method_get_flags_func>(
                    il2cpp_method_get_flags_ptr)(pMethod->address, &iflags);
            }
        }
        else
        {
            if (mono_method_get_flags_ptr)
            {
                using mono_method_get_flags_func = uint32_t(*)(void*, uint32_t*);
                uint32_t iflags = 0;
                flags = static_cast<mono_method_get_flags_func>(mono_method_get_flags_ptr)(pMethod->address, &iflags);
            }
        }

        cache.methodFlagsCache[pMethod] = flags;
        return flags;
    }

    static bool isEnum(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return false;

        auto it = cache.isEnumCache.find(pClass);
        if (it != cache.isEnumCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_is_enum_ptr)
            {
                using il2cpp_class_is_enum_func = bool(*)(void*);
                result = static_cast<il2cpp_class_is_enum_func>(il2cpp_class_is_enum_ptr)(pClass->address);
            }
        }
        else
        {
            if (mono_class_is_enum_ptr)
            {
                using mono_class_is_enum_func = bool(*)(void*);
                result = static_cast<mono_class_is_enum_func>(mono_class_is_enum_ptr)(pClass->address);
            }
        }

        if (!result)
        {
            result = (pClass->parent == "System.Enum");
        }

        cache.isEnumCache[pClass] = result;
        return result;
    }

    static bool isValueType(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return false;

        auto it = cache.isValueTypeCache.find(pClass);
        if (it != cache.isValueTypeCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_is_valuetype_ptr)
            {
                using il2cpp_class_is_valuetype_func = bool(*)(void*);
                result = static_cast<il2cpp_class_is_valuetype_func>(il2cpp_class_is_valuetype_ptr)(pClass->address);
            }
        }
        else
        {
            if (mono_class_is_valuetype_ptr)
            {
                using mono_class_is_valuetype_func = bool(*)(void*);
                result = static_cast<mono_class_is_valuetype_func>(mono_class_is_valuetype_ptr)(pClass->address);
            }
        }

        if (!result)
        {
            result = (pClass->parent == "System.ValueType" || isEnum(pClass));
        }

        cache.isValueTypeCache[pClass] = result;
        return result;
    }

    static bool isAbstract(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return false;

        auto it = cache.isAbstractCache.find(pClass);
        if (it != cache.isAbstractCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_is_abstract_ptr)
            {
                using il2cpp_class_is_abstract_func = bool(*)(void*);
                result = static_cast<il2cpp_class_is_abstract_func>(il2cpp_class_is_abstract_ptr)(pClass->address);
            }
        }
        else
        {
            // mono_class_is_abstract
            result = false;
        }

        if (!result)
        {
            int flags = getClassFlags(pClass);
            result = (flags & TYPE_ATTRIBUTE_ABSTRACT) != 0;
        }

        cache.isAbstractCache[pClass] = result;
        return result;
    }

    static bool isInterface(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return false;

        auto it = cache.isInterfaceCache.find(pClass);
        if (it != cache.isInterfaceCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_is_interface_ptr)
            {
                using il2cpp_class_is_interface_func = bool(*)(void*);
                result = static_cast<il2cpp_class_is_interface_func>(il2cpp_class_is_interface_ptr)(pClass->address);
            }
        }
        else
        {
            // mono_class_is_interface
            result = false;
        }

        if (!result)
        {
            int flags = getClassFlags(pClass);
            result = (flags & TYPE_ATTRIBUTE_INTERFACE) != 0;
        }

        cache.isInterfaceCache[pClass] = result;
        return result;
    }

    static bool isGenericClass(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return false;

        auto it = cache.isGenericClassCache.find(pClass);
        if (it != cache.isGenericClassCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_is_generic_ptr)
            {
                using il2cpp_class_is_generic_func = bool(*)(void*);
                result = static_cast<il2cpp_class_is_generic_func>(il2cpp_class_is_generic_ptr)(pClass->address);
            }
        }
        else
        {
            if (mono_class_is_generic_ptr)
            {
                using mono_class_is_generic_func = bool(*)(void*);
                result = static_cast<mono_class_is_generic_func>(mono_class_is_generic_ptr)(pClass->address);
            }
        }

        if (!result)
        {
            result = (pClass->name.find("`") != std::string::npos);
        }

        cache.isGenericClassCache[pClass] = result;
        return result;
    }

    static bool isGenericMethod(UnityResolve::Method* pMethod)
    {
        if (!pMethod || !pMethod->address) return false;

        auto it = cache.isGenericMethodCache.find(pMethod);
        if (it != cache.isGenericMethodCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_method_is_generic_ptr)
            {
                using il2cpp_method_is_generic_func = bool(*)(void*);
                result = static_cast<il2cpp_method_is_generic_func>(il2cpp_method_is_generic_ptr)(pMethod->address);
            }
        }
        else
        {
            if (unity_mono_method_is_generic_ptr)
            {
                using unity_mono_method_is_generic_func = bool(*)(void*);
                result = static_cast<unity_mono_method_is_generic_func>(unity_mono_method_is_generic_ptr)(
                    pMethod->address);
            }
        }

        cache.isGenericMethodCache[pMethod] = result;
        return result;
    }

    static bool isLiteralField(UnityResolve::Field* pField)
    {
        if (!pField || !pField->address) return false;

        auto it = cache.isLiteralFieldCache.find(pField);
        if (it != cache.isLiteralFieldCache.end())
        {
            return it->second;
        }

        bool result = false;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_field_is_literal_ptr)
            {
                using il2cpp_field_is_literal_func = bool(*)(void*);
                result = static_cast<il2cpp_field_is_literal_func>(il2cpp_field_is_literal_ptr)(pField->address);
            }
        }
        else
        {
            if (mono_unity_class_field_is_literal_ptr)
            {
                using mono_unity_class_field_is_literal_func = bool(*)(void*);
                result = static_cast<mono_unity_class_field_is_literal_func>(mono_unity_class_field_is_literal_ptr)(
                    pField->address);
            }
        }

        if (!result)
        {
            int flags = getFieldFlags(pField);
            result = (flags & FIELD_ATTRIBUTE_LITERAL) != 0;
        }

        cache.isLiteralFieldCache[pField] = result;
        return result;
    }

    // Cached class instance size lookup
    static int32_t getClassInstanceSize(UnityResolve::Class* pClass)
    {
        if (!pClass || !pClass->address) return 0;

        auto it = cache.classInstanceSizeCache.find(pClass);
        if (it != cache.classInstanceSizeCache.end())
        {
            return it->second;
        }

        int32_t size = 0;
        if (UnityResolve::mode_ == UnityResolve::Mode::Il2Cpp)
        {
            if (il2cpp_class_instance_size_ptr)
            {
                using il2cpp_class_instance_size_func = int32_t(*)(void*);
                size = static_cast<il2cpp_class_instance_size_func>(il2cpp_class_instance_size_ptr)(pClass->address);
            }
        }
        else
        {
            if (mono_class_instance_size_ptr)
            {
                using mono_class_instance_size_func = int32_t(*)(void*);
                size = static_cast<mono_class_instance_size_func>(mono_class_instance_size_ptr)(pClass->address);
            }
        }

        cache.classInstanceSizeCache[pClass] = size;
        return size;
    }

    static std::string sanitizeName(const std::string& name)
    {
        // Check cache first
        auto it = cache.sanitizedNameCache.find(name);
        if (it != cache.sanitizedNameCache.end())
        {
            return it->second;
        }

        std::string result = name;

        // Replace invalid C++ identifier characters
        result = std::regex_replace(result, cache.invalidCharsRegex, "_");
        result = std::regex_replace(result, cache.multiUnderscoreRegex, "_");

        // Remove consecutive underscores
        std::regex multiUnderscore("_{2,}");
        result = std::regex_replace(result, multiUnderscore, "_");

        // Remove leading/trailing underscores
        if (!result.empty() && result.front() == '_') result.erase(0, 1);
        if (!result.empty() && result.back() == '_') result.pop_back();

        // Ensure it doesn't start with a number
        if (!result.empty() && std::isdigit(result[0]))
        {
            result = "n" + result;
        }

        // Handle empty names
        if (result.empty()) result = "unnamed";

        // Cache the result
        cache.sanitizedNameCache[name] = result;
        return result;
    }

    static std::string getClassModifiers(UnityResolve::Class* pClass)
    {
        std::string modifiers = "";

        int flags = getClassFlags(pClass);

        if (flags & TYPE_ATTRIBUTE_SEALED)
        {
            modifiers += " final";
        }

        return modifiers;
    }

    static int64_t getEnumValue(UnityResolve::Field* pField, UnityResolve::Class* pClass)
    {
        if (isLiteralField(pField))
        {
            int64_t value = 0;
            pField->GetStaticValue(&value);
            return value;
        }
        return 0;
    }

    static std::string extractEnumLiteral(UnityResolve::Field* pField)
    {
        if (!isLiteralField(pField) || !pField->klass) return "0";

        try
        {
            std::string numericValue = extractNumericLiteral(pField);

            if (isEnum(pField->klass))
            {
                // Look for a field with the same value in the enum class
                for (const auto& enumField : pField->klass->fields)
                {
                    if (isLiteralField(enumField))
                    {
                        int64_t enumFieldValue = getEnumValue(enumField, pField->klass);
                        if (std::to_string(enumFieldValue) == numericValue)
                        {
                            auto className = getTypeMapping(pField->klass->name);
                            auto lit = className + "::" + sanitizeName(enumField->name);
                            // LOG_DEBUG("Found enum literal: {} = {}", lit, numericValue);
                            return lit;
                        }
                    }
                }
            }

            // Return numeric value if no name found
            return numericValue;
        }
        catch (...)
        {
            return "0";
        }
    }

public:
    static void DumpToCppHeaders(const std::string& outputPath)
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        LOG_INFO("Starting dump...");

        initializeApiPointers();
        buildClassAssemblyMapping();

        std::ofstream mainHeader(outputPath + "UnityClasses.hpp");
        if (!mainHeader.is_open())
        {
            LOG_ERROR("Failed to create output file: {}", outputPath + "UnityClasses.hpp");
            return;
        }

        mainHeader << "#pragma once\n";
        mainHeader << "#include \"UnityResolve.hpp\"\n";
        mainHeader << "#include \"UnityMacros.hpp\"\n";
        mainHeader << "#include <cstdint>\n";
        mainHeader << "#include <string>\n";
        mainHeader << "#include <vector>\n";
        mainHeader << "#include <memory>\n\n";

        std::vector<UnityResolve::Class*> allClasses;
        allClasses.reserve(cache.totalClasses);

        for (const auto& pAssembly : UnityResolve::assembly)
        {
            for (const auto& pClass : pAssembly->classes)
            {
                allClasses.push_back(pClass);
            }
        }

        LOG_INFO("Processing {} classes...", allClasses.size());

        mainHeader << "// Forward declarations\n";
        for (const auto& pClass : allClasses)
        {
            if (isEnum(pClass))
            {
                mainHeader << "enum class " << sanitizeName(pClass->name) << "_Enum;\n";
            }
            else if (isGenericClass(pClass))
            {
                GenericInfo genericInfo = parseGenericType(pClass->name);
                if (genericInfo.isGeneric)
                {
                    mainHeader << "template<";
                    for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
                    {
                        if (i > 0) mainHeader << ", ";
                        mainHeader << "typename " << genericInfo.parameterNames[i];
                    }
                    mainHeader << "> class " << sanitizeName(genericInfo.baseName) << ";\n";
                }
            }
            else
            {
                mainHeader << "class " << sanitizeName(pClass->name) << ";\n";
            }
        }
        mainHeader << "\n";

        size_t processedCount = 0;
        for (const auto& pClass : allClasses)
        {
            if (isEnum(pClass))
            {
                DumpEnum(mainHeader, pClass);
                LOG_INFO("Dumped enum: {} ({}/{})", pClass->name.c_str(), ++processedCount, allClasses.size());
            }
            else if (isGenericClass(pClass))
            {
                DumpGenericClass(mainHeader, pClass);
                LOG_INFO("Dumped generic class: {} ({}/{})", pClass->name.c_str(), ++processedCount, allClasses.size());
            }
            else
            {
                DumpClass(mainHeader, pClass);
                LOG_INFO("Dumped class: {} ({}/{})", pClass->name.c_str(), ++processedCount, allClasses.size());
            }
        }

        mainHeader.close();

        CreateMacrosHeader(outputPath + "UnityMacros.hpp");
        CreateHelperFunctions(outputPath + "UnityHelpers.hpp");

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        LOG_INFO("Dump completed in {} ms", duration.count());
    }

private:
    static void DumpEnum(std::ofstream& out, UnityResolve::Class* pClass)
    {
        std::string enumName = sanitizeName(pClass->name);
        if (enumName.empty()) return;

        int classFlags = getClassFlags(pClass);
        std::string underlyingType = "int32_t";
        std::string assemblyName = getAssemblyNameForClass(pClass);

        out << "// Assembly: " << assemblyName << "\n";
        out << "// Namespace: " << (pClass->namespaze.empty() ? "Global" : pClass->namespaze) << "\n";
        out << "// Class Flags: 0x" << std::hex << classFlags << std::dec << "\n";
        out << "// Instance Size: " << getClassInstanceSize(pClass) << "\n";
        out << "enum class " << enumName << "_Enum : " << underlyingType << " {\n";

        std::vector<std::pair<std::string, int64_t>> enumValues;

        for (const auto& pField : pClass->fields)
        {
            int fieldFlags = getFieldFlags(pField);

            if (pField->name == "value__") continue;

            if ((fieldFlags & FIELD_ATTRIBUTE_STATIC) && (fieldFlags & FIELD_ATTRIBUTE_LITERAL))
            {
                std::string fieldName = sanitizeName(pField->name);
                if (!fieldName.empty())
                {
                    int64_t value = getEnumValue(pField, pClass);
                    enumValues.push_back({fieldName, value});
                }
            }
        }

        std::sort(enumValues.begin(), enumValues.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });

        for (const auto& [name, value] : enumValues)
        {
            out << "    " << name << " = " << value << ",\n";
        }

        out << "};\n\n";
    }

    static void DumpGenericClass(std::ofstream& out, UnityResolve::Class* pClass)
    {
        std::string className = sanitizeName(pClass->name);
        if (className.empty()) return;

        GenericInfo genericInfo = parseGenericType(pClass->name);
        if (!genericInfo.isGeneric) return;

        std::string assemblyName = getAssemblyNameForClass(pClass);
        int classFlags = getClassFlags(pClass);
        bool isInterfaceClass = isInterface(pClass);
        int32_t instanceSize = getClassInstanceSize(pClass);

        out << "// Assembly: " << assemblyName << "\n";
        out << "// Namespace: " << (pClass->namespaze.empty() ? "Global" : pClass->namespaze) << "\n";
        out << "// Generic Class: " << pClass->name << "\n";
        out << "// Generic Parameters: " << genericInfo.parameterCount << "\n";
        out << "// Class Flags: 0x" << std::hex << classFlags << std::dec << "\n";
        out << "// Instance Size: " << instanceSize << " bytes\n";

        // Generate template declaration
        out << "template<";
        for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
        {
            if (i > 0) out << ", ";
            out << "typename " << genericInfo.parameterNames[i];
        }
        out << ">\n";

        if (isInterfaceClass)
        {
            out << "struct " << sanitizeName(genericInfo.baseName);
        }
        else
        {
            out << "class " << sanitizeName(genericInfo.baseName);
        }

        // Handle inheritance
        if (!pClass->parent.empty() && pClass->parent != "System.Object")
        {
            std::string parentType = getTypeMapping(pClass->parent);
            if (!parentType.empty() && parentType.back() == '*')
            {
                parentType.pop_back();
            }
            out << " : public " << parentType;
        }

        out << " {\n";

        // Unity class declaration macro for generics
        out << "    UNITY_GENERIC_CLASS_DECL(\"" << assemblyName << "\", \"" << pClass->name << "\")\n\n";

        // Track method overloads for this class
        std::unordered_map<std::string, int> methodCounts;

        // Fields
        std::vector<UnityResolve::Field*> fields;
        std::vector<UnityResolve::Field*> staticFields;

        for (const auto& pField : pClass->fields)
        {
            int fieldFlags = getFieldFlags(pField);
            if (fieldFlags & FIELD_ATTRIBUTE_STATIC)
            {
                staticFields.push_back(pField);
            }
            else
            {
                fields.push_back(pField);
            }
        }

        if (!fields.empty())
        {
            out << "public:\n";
            out << "    // Fields\n";
            for (const auto& pField : fields)
            {
                DumpGenericField(out, pField, genericInfo);
            }
            out << "\n";
        }

        // Methods
        std::vector<UnityResolve::Method*> methods;
        std::vector<UnityResolve::Method*> staticMethods;

        for (const auto& pMethod : pClass->methods)
        {
            uint32_t methodFlags = getMethodFlags(pMethod);
            if (methodFlags & METHOD_ATTRIBUTE_STATIC)
            {
                staticMethods.push_back(pMethod);
            }
            else
            {
                methods.push_back(pMethod);
            }
        }

        if (!methods.empty())
        {
            out << "public:\n";
            out << "    // Methods\n";
            for (const auto& pMethod : methods)
            {
                methodCounts[pMethod->name]++;
                std::string uniqueName = getUniqueMethodName(pClass, pMethod, methodCounts);
                DumpGenericMethod(out, pMethod, genericInfo, uniqueName);
            }
            out << "\n";
        }

        // Static fields and methods
        if (!staticFields.empty())
        {
            out << "public:\n";
            out << "    // Static Fields\n";
            for (const auto& pField : staticFields)
            {
                DumpGenericStaticField(out, pField, genericInfo);
            }
            out << "\n";
        }

        if (!staticMethods.empty())
        {
            out << "public:\n";
            out << "    // Static Methods\n";
            for (const auto& pMethod : staticMethods)
            {
                methodCounts[pMethod->name]++;
                std::string uniqueName = getUniqueMethodName(pClass, pMethod, methodCounts);
                DumpGenericStaticMethod(out, pMethod, genericInfo, uniqueName);
            }
            out << "\n";
        }

        out << "};\n\n";
    }

    static void DumpClass(std::ofstream& out, UnityResolve::Class* pClass)
    {
        std::string className = sanitizeName(pClass->name);
        if (className.empty()) return;

        int classFlags = getClassFlags(pClass);
        bool isAbstractClass = isAbstract(pClass);
        bool isInterfaceClass = isInterface(pClass);
        int32_t instanceSize = getClassInstanceSize(pClass);
        std::string assemblyName = getAssemblyNameForClass(pClass);

        out << "// Assembly: " << assemblyName << "\n";
        out << "// Namespace: " << (pClass->namespaze.empty() ? "Global" : pClass->namespaze) << "\n";
        out << "// Class Flags: 0x" << std::hex << classFlags << std::dec;

        std::vector<std::string> flagDescriptions;
        if (classFlags & TYPE_ATTRIBUTE_PUBLIC) flagDescriptions.push_back("Public");
        if (classFlags & TYPE_ATTRIBUTE_ABSTRACT) flagDescriptions.push_back("Abstract");
        if (classFlags & TYPE_ATTRIBUTE_SEALED) flagDescriptions.push_back("Sealed");
        if (classFlags & TYPE_ATTRIBUTE_INTERFACE) flagDescriptions.push_back("Interface");
        if (classFlags & TYPE_ATTRIBUTE_SERIALIZABLE) flagDescriptions.push_back("Serializable");

        if (!flagDescriptions.empty())
        {
            out << " (";
            for (size_t i = 0; i < flagDescriptions.size(); ++i)
            {
                if (i > 0) out << ", ";
                out << flagDescriptions[i];
            }
            out << ")";
        }
        out << "\n";

        out << "// Instance Size: " << instanceSize << " bytes\n";
        out << "// Is Abstract: " << (isAbstractClass ? "true" : "false") << "\n";
        out << "// Is Interface: " << (isInterfaceClass ? "true" : "false") << "\n";

        if (isInterfaceClass)
        {
            out << "struct " << className;
        }
        else
        {
            std::string classModifiers = getClassModifiers(pClass);
            out << "class " << className << classModifiers;
        }

        // Handle inheritance
        std::string parentClass = "";
        if (!pClass->parent.empty() && pClass->parent != "System.Object")
        {
            parentClass = getTypeMapping(pClass->parent);
            if (!parentClass.empty() && parentClass.back() == '*')
            {
                parentClass.pop_back();
            }
            out << " : public " << parentClass;
        }

        out << " {\n";

        // Unity class declaration macro
        out << "    UNITY_CLASS_DECL(\"" << assemblyName << "\", \"" << pClass->name << "\")\n\n";

        // Track method overloads for this class
        std::unordered_map<std::string, int> methodCounts;

        // Fields
        std::vector<UnityResolve::Field*> fields;
        std::vector<UnityResolve::Field*> staticFields;

        for (const auto& pField : pClass->fields)
        {
            int fieldFlags = getFieldFlags(pField);
            if (fieldFlags & FIELD_ATTRIBUTE_STATIC)
            {
                staticFields.push_back(pField);
            }
            else
            {
                fields.push_back(pField);
            }
        }

        if (!fields.empty())
        {
            out << "public:\n";
            out << "    // Fields\n";
            for (const auto& pField : fields)
            {
                DumpField(out, pField);
            }
            out << "\n";
        }

        // Methods with overload handling
        std::vector<UnityResolve::Method*> methods;
        std::vector<UnityResolve::Method*> staticMethods;

        for (const auto& pMethod : pClass->methods)
        {
            uint32_t methodFlags = getMethodFlags(pMethod);
            if (methodFlags & METHOD_ATTRIBUTE_STATIC)
            {
                staticMethods.push_back(pMethod);
            }
            else
            {
                methods.push_back(pMethod);
            }
        }

        if (!methods.empty())
        {
            out << "public:\n";
            out << "    // Methods\n";
            for (const auto& pMethod : methods)
            {
                methodCounts[pMethod->name]++;
                std::string uniqueName = getUniqueMethodName(pClass, pMethod, methodCounts);
                DumpMethod(out, pMethod, uniqueName);
            }
            out << "\n";
        }

        // Static fields and methods
        if (!staticFields.empty())
        {
            out << "public:\n";
            out << "    // Static Fields\n";
            for (const auto& pField : staticFields)
            {
                DumpStaticField(out, pField);
            }
            out << "\n";
        }

        if (!staticMethods.empty())
        {
            out << "public:\n";
            out << "    // Static Methods\n";
            for (const auto& pMethod : staticMethods)
            {
                methodCounts[pMethod->name]++;
                std::string uniqueName = getUniqueMethodName(pClass, pMethod, methodCounts);
                DumpStaticMethod(out, pMethod, uniqueName);
            }
            out << "\n";
        }

        out << "};\n\n";
    }

    static void DumpField(std::ofstream& out, UnityResolve::Field* pField)
    {
        std::string fieldName = sanitizeName(pField->name);
        std::string fieldType = getTypeMapping(pField->type->name);

        if (fieldName.empty()) return;

        int fieldFlags = getFieldFlags(pField);

        out << "    // Offset: 0x" << std::hex << pField->offset << std::dec;
        out << ", Flags: 0x" << std::hex << fieldFlags << std::dec;
        out << ", Type: " << pField->type->name;

        std::vector<std::string> flagDescriptions;
        if (fieldFlags & FIELD_ATTRIBUTE_STATIC) flagDescriptions.push_back("Static");
        if (fieldFlags & FIELD_ATTRIBUTE_INIT_ONLY) flagDescriptions.push_back("ReadOnly");
        if (fieldFlags & FIELD_ATTRIBUTE_LITERAL) flagDescriptions.push_back("Literal");
        if (fieldFlags & FIELD_ATTRIBUTE_NOT_SERIALIZED) flagDescriptions.push_back("NotSerialized");

        if (!flagDescriptions.empty())
        {
            out << " (";
            for (size_t i = 0; i < flagDescriptions.size(); ++i)
            {
                if (i > 0) out << ", ";
                out << flagDescriptions[i];
            }
            out << ")";
        }
        out << "\n";

        // Handle literal fields
        if (fieldFlags & FIELD_ATTRIBUTE_LITERAL)
        {
            std::string literalValue = getLiteralValueWithType(pField);
            if (pField->type->name == "System.String")
            {
                out << "    UNITY_STRING_LITERAL(" << fieldName << ", " << literalValue << ")\n";
            }
            else
            {
                out << "    UNITY_LITERAL_FIELD(" << fieldType << ", " << fieldName << ", " << literalValue << ")\n";
            }
        }
        else
        {
            out << "    UNITY_FIELD(" << fieldType << ", " << fieldName << ", 0x"
                << std::hex << pField->offset << std::dec << ")\n";
        }
    }

    static void DumpStaticField(std::ofstream& out, UnityResolve::Field* pField)
    {
        std::string fieldName = sanitizeName(pField->name);
        std::string fieldType = getTypeMapping(pField->type->name);

        if (fieldName.empty()) return;

        int fieldFlags = getFieldFlags(pField);

        out << "    // Static Field - Flags: 0x" << std::hex << fieldFlags << std::dec;
        out << ", Type: " << pField->type->name;

        if (fieldFlags & FIELD_ATTRIBUTE_LITERAL)
        {
            out << ", Literal Value: " << pField->offset;
        }
        out << "\n";

        if (fieldFlags & FIELD_ATTRIBUTE_LITERAL)
        {
            std::string literalValue = getLiteralValueWithType(pField);
            if (pField->type->name == "System.String")
            {
                out << "    UNITY_STATIC_STRING_LITERAL(" << fieldName << ", " << literalValue << ")\n";
            }
            else
            {
                out << "    UNITY_LITERAL_FIELD(" << fieldType << ", " << fieldName << ", " << literalValue << ")\n";
            }
        }
        else
        {
            out << "    UNITY_STATIC_FIELD(" << fieldType << ", " << fieldName << ")\n";
        }
    }

    static void DumpMethod(std::ofstream& out, UnityResolve::Method* pMethod, const std::string& uniqueName = "")
    {
        std::string methodName = uniqueName.empty() ? sanitizeName(pMethod->name) : uniqueName;
        std::string returnType = getTypeMapping(pMethod->return_type->name);

        if (methodName.empty()) return;

        // Skip for now
        if (methodName == "ctor" || methodName == "dtor" || methodName == "Finalize" || methodName == "cctor")
        {
            return;
        }

        uint32_t methodFlags = getMethodFlags(pMethod);
        bool isGeneric = isGenericMethod(pMethod);

        out << "    // Flags: 0x" << std::hex << methodFlags << std::dec;
        if (pMethod->function)
        {
            out << ", RVA: 0x" << std::hex << (reinterpret_cast<std::uint64_t>(pMethod->function) -
                reinterpret_cast<std::uint64_t>(UnityResolve::hmodule_)) << std::dec;
        }
        out << ", ParamCount: " << pMethod->args.size();
        out << ", ReturnType: " << pMethod->return_type->name;

        std::vector<std::string> flagDescriptions;
        if (methodFlags & METHOD_ATTRIBUTE_STATIC) flagDescriptions.push_back("Static");
        if (methodFlags & METHOD_ATTRIBUTE_VIRTUAL) flagDescriptions.push_back("Virtual");
        if (methodFlags & METHOD_ATTRIBUTE_ABSTRACT) flagDescriptions.push_back("Abstract");
        if (methodFlags & METHOD_ATTRIBUTE_FINAL) flagDescriptions.push_back("Final");
        if (isGeneric) flagDescriptions.push_back("Generic");

        if (!flagDescriptions.empty())
        {
            out << " (";
            for (size_t i = 0; i < flagDescriptions.size(); ++i)
            {
                if (i > 0) out << ", ";
                out << flagDescriptions[i];
            }
            out << ")";
        }
        out << "\n";

        if (!pMethod->args.empty())
        {
            out << "    // Parameters: ";
            for (size_t i = 0; i < pMethod->args.size(); ++i)
            {
                if (i > 0) out << ", ";
                out << getTypeMapping(pMethod->args[i]->pType->name) << " " << sanitizeName(pMethod->args[i]->name);
            }
            out << "\n";
        }

        if (isGeneric)
        {
            out << "    UNITY_GENERIC_METHOD(" << returnType << ", " << methodName;
            if (!pMethod->args.empty())
            {
                for (const auto& arg : pMethod->args)
                {
                    out << ", " << getTypeMapping(arg->pType->name);
                }
            }
            out << ")\n";
        }
        else if (pMethod->args.empty())
        {
            out << "    UNITY_METHOD(" << returnType << ", " << methodName << ")\n";
        }
        else
        {
            out << "    UNITY_METHOD(" << returnType << ", " << methodName;
            for (const auto& arg : pMethod->args)
            {
                out << ", " << getTypeMapping(arg->pType->name);
            }
            out << ")\n";
        }
    }

    static void DumpStaticMethod(std::ofstream& out, UnityResolve::Method* pMethod, const std::string& uniqueName = "")
    {
        std::string methodName = uniqueName.empty() ? sanitizeName(pMethod->name) : uniqueName;
        std::string returnType = getTypeMapping(pMethod->return_type->name);

        if (methodName.empty()) return;

        if (methodName == "cctor") return;

        uint32_t methodFlags = getMethodFlags(pMethod);
        bool isGeneric = isGenericMethod(pMethod);

        out << "    // Static Method - Flags: 0x" << std::hex << methodFlags << std::dec;
        if (pMethod->function)
        {
            out << ", RVA: 0x" << std::hex << (reinterpret_cast<std::uint64_t>(pMethod->function) -
                reinterpret_cast<std::uint64_t>(UnityResolve::hmodule_)) << std::dec;
        }
        out << ", ParamCount: " << pMethod->args.size();
        if (isGeneric) out << " (Generic)";
        out << "\n";

        if (isGeneric)
        {
            out << "    UNITY_STATIC_GENERIC_METHOD(" << returnType << ", " << methodName;
            if (!pMethod->args.empty())
            {
                for (const auto& arg : pMethod->args)
                {
                    out << ", " << getTypeMapping(arg->pType->name);
                }
            }
            out << ")\n";
        }
        else if (pMethod->args.empty())
        {
            out << "    UNITY_STATIC_METHOD(" << returnType << ", " << methodName << ")\n";
        }
        else
        {
            out << "    UNITY_STATIC_METHOD(" << returnType << ", " << methodName;
            for (const auto& arg : pMethod->args)
            {
                out << ", " << getTypeMapping(arg->pType->name);
            }
            out << ")\n";
        }
    }

    // Generic specific dump methods
    static void DumpGenericField(std::ofstream& out, UnityResolve::Field* pField, const GenericInfo& genericInfo)
    {
        std::string fieldName = sanitizeName(pField->name);
        std::string fieldType = getTypeMapping(pField->type->name);

        // Replace generic type parameters in field type
        for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
        {
            std::string placeholder = "T" + std::to_string(i);
            if (fieldType.find(placeholder) != std::string::npos)
            {
                fieldType = std::regex_replace(fieldType, std::regex(placeholder), genericInfo.parameterNames[i]);
            }
        }

        if (fieldName.empty()) return;

        out << "    // Offset: 0x" << std::hex << pField->offset << std::dec;
        out << ", Type: " << pField->type->name << "\n";

        out << "    UNITY_GENERIC_FIELD(" << fieldType << ", " << fieldName << ", 0x"
            << std::hex << pField->offset << std::dec << ")\n";
    }

    static void DumpGenericStaticField(std::ofstream& out, UnityResolve::Field* pField, const GenericInfo& genericInfo)
    {
        std::string fieldName = sanitizeName(pField->name);
        std::string fieldType = getTypeMapping(pField->type->name);

        // Replace generic type parameters
        for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
        {
            std::string placeholder = "T" + std::to_string(i);
            if (fieldType.find(placeholder) != std::string::npos)
            {
                fieldType = std::regex_replace(fieldType, std::regex(placeholder), genericInfo.parameterNames[i]);
            }
        }

        if (fieldName.empty()) return;

        int fieldFlags = getFieldFlags(pField);

        out << "    // Static Generic Field - Type: " << pField->type->name << "\n";

        if (fieldFlags & FIELD_ATTRIBUTE_LITERAL)
        {
            std::string literalValue = getLiteralValueWithType(pField);
            out << "    UNITY_GENERIC_LITERAL_FIELD(" << fieldType << ", " << fieldName << ", " << literalValue <<
                ")\n";
        }
        else
        {
            out << "    UNITY_GENERIC_STATIC_FIELD(" << fieldType << ", " << fieldName << ")\n";
        }
    }

    static void DumpGenericMethod(std::ofstream& out, UnityResolve::Method* pMethod, const GenericInfo& genericInfo,
                                  const std::string& uniqueName)
    {
        std::string methodName = uniqueName.empty() ? sanitizeName(pMethod->name) : uniqueName;
        std::string returnType = getTypeMapping(pMethod->return_type->name);

        if (methodName.empty()) return;
        if (methodName == "ctor" || methodName == "dtor" || methodName == "Finalize" || methodName == "cctor") return;

        // Replace generic type parameters in return type
        for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
        {
            std::string placeholder = "T" + std::to_string(i);
            if (returnType.find(placeholder) != std::string::npos)
            {
                returnType = std::regex_replace(returnType, std::regex(placeholder), genericInfo.parameterNames[i]);
            }
        }

        uint32_t methodFlags = getMethodFlags(pMethod);
        bool isGeneric = isGenericMethod(pMethod);

        out << "    // Generic Method - Flags: 0x" << std::hex << methodFlags << std::dec;
        out << ", ParamCount: " << pMethod->args.size();
        if (isGeneric) out << " (Method-level Generic)";
        out << "\n";

        if (!pMethod->args.empty())
        {
            out << "    // Parameters: ";
            for (size_t i = 0; i < pMethod->args.size(); ++i)
            {
                if (i > 0) out << ", ";
                std::string argType = getTypeMapping(pMethod->args[i]->pType->name);

                // Replace generic parameters in argument types
                for (size_t j = 0; j < genericInfo.parameterNames.size(); ++j)
                {
                    std::string placeholder = "T" + std::to_string(j);
                    if (argType.find(placeholder) != std::string::npos)
                    {
                        argType = std::regex_replace(argType, std::regex(placeholder), genericInfo.parameterNames[j]);
                    }
                }

                out << argType << " " << sanitizeName(pMethod->args[i]->name);
            }
            out << "\n";
        }

        if (pMethod->args.empty())
        {
            out << "    UNITY_GENERIC_METHOD(" << returnType << ", " << methodName << ")\n";
        }
        else
        {
            out << "    UNITY_GENERIC_METHOD(" << returnType << ", " << methodName;
            for (const auto& arg : pMethod->args)
            {
                std::string argType = getTypeMapping(arg->pType->name);

                // Replace generic parameters
                for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
                {
                    std::string placeholder = "T" + std::to_string(i);
                    if (argType.find(placeholder) != std::string::npos)
                    {
                        argType = std::regex_replace(argType, std::regex(placeholder), genericInfo.parameterNames[i]);
                    }
                }

                out << ", " << argType;
            }
            out << ")\n";
        }
    }

    static void DumpGenericStaticMethod(std::ofstream& out, UnityResolve::Method* pMethod,
                                        const GenericInfo& genericInfo, const std::string& uniqueName)
    {
        std::string methodName = uniqueName.empty() ? sanitizeName(pMethod->name) : uniqueName;
        std::string returnType = getTypeMapping(pMethod->return_type->name);

        if (methodName.empty()) return;
        if (methodName == "cctor") return;

        // Replace generic type parameters in return type
        for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
        {
            std::string placeholder = "T" + std::to_string(i);
            if (returnType.find(placeholder) != std::string::npos)
            {
                returnType = std::regex_replace(returnType, std::regex(placeholder), genericInfo.parameterNames[i]);
            }
        }

        uint32_t methodFlags = getMethodFlags(pMethod);
        bool isGeneric = isGenericMethod(pMethod);

        out << "    // Static Generic Method - Flags: 0x" << std::hex << methodFlags << std::dec;
        out << ", ParamCount: " << pMethod->args.size();
        if (isGeneric) out << " (Method-level Generic)";
        out << "\n";

        if (pMethod->args.empty())
        {
            out << "    UNITY_GENERIC_STATIC_METHOD(" << returnType << ", " << methodName << ")\n";
        }
        else
        {
            out << "    UNITY_GENERIC_STATIC_METHOD(" << returnType << ", " << methodName;
            for (const auto& arg : pMethod->args)
            {
                std::string argType = getTypeMapping(arg->pType->name);

                // Replace generic parameters
                for (size_t i = 0; i < genericInfo.parameterNames.size(); ++i)
                {
                    std::string placeholder = "T" + std::to_string(i);
                    if (argType.find(placeholder) != std::string::npos)
                    {
                        argType = std::regex_replace(argType, std::regex(placeholder), genericInfo.parameterNames[i]);
                    }
                }

                out << ", " << argType;
            }
            out << ")\n";
        }
    }

    static void CreateMacrosHeader(const std::string& filepath)
    {
        std::ofstream out(filepath);
        if (!out.is_open()) return;

        out << R"(#pragma once
#include "UnityResolve.hpp"
#include <unordered_map>

#define UNITY_CLASS_DECL(MODULE, CLASS_NAME) \
private:\
    inline static constexpr const char* ModuleName = MODULE; \
    inline static constexpr const char* ClassName = CLASS_NAME; \
public: \
    inline static const char* getClassName() { return ClassName; } \
    inline static const char* getModuleName() { return ModuleName; } \
    inline static UnityResolve::Class* getClass() { \
        static UnityResolve::Class* c = nullptr; \
        if (!c) c = app::getClass(MODULE, CLASS_NAME); \
        return c; \
    }

#define UNITY_GENERIC_CLASS_DECL(MODULE, CLASS_NAME) \
private:\
    inline static constexpr const char* ModuleName = MODULE; \
    inline static constexpr const char* ClassName = CLASS_NAME; \
public: \
    inline static const char* getClassName() { return ClassName; } \
    inline static const char* getModuleName() { return ModuleName; } \
    inline static UnityResolve::Class* getClass() { \
        static UnityResolve::Class* c = nullptr; \
        if (!c) c = app::getClass(MODULE, CLASS_NAME); \
        return c; \
    }

#define UNITY_FIELD(FIELD_TYPE, FIELD_NAME, FIELD_OFFSET) \
    inline FIELD_TYPE FIELD_NAME() { \
        return *reinterpret_cast<FIELD_TYPE*>(reinterpret_cast<uintptr_t>(this) + FIELD_OFFSET); \
    } \
    inline void FIELD_NAME(FIELD_TYPE value) const { \
        *reinterpret_cast<FIELD_TYPE*>(reinterpret_cast<uintptr_t>(this) + FIELD_OFFSET) = value; \
    }

#define UNITY_GENERIC_FIELD(FIELD_TYPE, FIELD_NAME, FIELD_OFFSET) \
    inline FIELD_TYPE FIELD_NAME() { \
        return *reinterpret_cast<FIELD_TYPE*>(reinterpret_cast<uintptr_t>(this) + FIELD_OFFSET); \
    } \
    inline void FIELD_NAME(FIELD_TYPE value) const { \
        *reinterpret_cast<FIELD_TYPE*>(reinterpret_cast<uintptr_t>(this) + FIELD_OFFSET) = value; \
    }

#define UNITY_STATIC_FIELD(FIELD_TYPE, FIELD_NAME) \
public: \
    inline static FIELD_TYPE FIELD_NAME() { \
        auto field = getClass()->Get<UnityResolve::Field>(#FIELD_NAME); \
        if (field && field->static_field) { \
            FIELD_TYPE value; \
            field->GetStaticValue(&value); \
            return value; \
        } \
        return FIELD_TYPE{}; \
    } \
    inline static void FIELD_NAME(FIELD_TYPE value) { \
        auto field = getClass()->Get<UnityResolve::Field>(#FIELD_NAME); \
        if (field && field->static_field) { \
            field->SetStaticValue(&value); \
        } \
    }

#define UNITY_GENERIC_STATIC_FIELD(FIELD_TYPE, FIELD_NAME) \
public: \
    inline static FIELD_TYPE FIELD_NAME() { \
        auto field = getClass()->Get<UnityResolve::Field>(#FIELD_NAME); \
        if (field && field->static_field) { \
            FIELD_TYPE value; \
            field->GetStaticValue(&value); \
            return value; \
        } \
        return FIELD_TYPE{}; \
    } \
    inline static void FIELD_NAME(FIELD_TYPE value) { \
        auto field = getClass()->Get<UnityResolve::Field>(#FIELD_NAME); \
        if (field && field->static_field) { \
            field->SetStaticValue(&value); \
        } \
    }

#define UNITY_LITERAL_FIELD(FIELD_TYPE, FIELD_NAME, LITERAL_VALUE) \
public: \
    inline static constexpr FIELD_TYPE FIELD_NAME() { \
        return static_cast<FIELD_TYPE>(LITERAL_VALUE); \
    }

#define UNITY_GENERIC_LITERAL_FIELD(FIELD_TYPE, FIELD_NAME, LITERAL_VALUE) \
public: \
    inline static constexpr FIELD_TYPE FIELD_NAME() { \
        return static_cast<FIELD_TYPE>(LITERAL_VALUE); \
    }

#define UNITY_STRING_LITERAL(FIELD_NAME, LITERAL_VALUE) \
public: \
    inline static constexpr const char* FIELD_NAME() { \
        return LITERAL_VALUE; \
    }

#define UNITY_STATIC_STRING_LITERAL(FIELD_NAME, LITERAL_VALUE) \
public: \
    inline static constexpr const char* FIELD_NAME() { \
        return LITERAL_VALUE; \
    }

#define UNITY_ENUM_LITERAL(FIELD_NAME, LITERAL_VALUE) \
public: \
    inline static constexpr auto FIELD_NAME() { \
        return LITERAL_VALUE; \
    }

#define UNITY_CONST_LITERAL(FIELD_TYPE, FIELD_NAME, LITERAL_VALUE) \
public: \
    inline static constexpr FIELD_TYPE FIELD_NAME = LITERAL_VALUE;

#define UNITY_METHOD(RETURN_TYPE, METHOD_NAME, ...) \
private: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_ptr{}; \
    inline static bool METHOD_NAME##_initialized = false; \
public: \
    inline UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME() { \
        if (!METHOD_NAME##_initialized) { \
            auto method = app::getMethod(getClass(), #METHOD_NAME); \
            if (method) { \
                METHOD_NAME##_ptr = method->Cast<RETURN_TYPE, __VA_ARGS__>(); \
            } \
            METHOD_NAME##_initialized = true; \
        } \
        return METHOD_NAME##_ptr; \
    }

#define UNITY_GENERIC_METHOD(RETURN_TYPE, METHOD_NAME, ...) \
private: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_ptr{}; \
    inline static bool METHOD_NAME##_initialized = false; \
public: \
    inline UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME() { \
        if (!METHOD_NAME##_initialized) { \
            auto method = app::getMethod(getClass(), #METHOD_NAME); \
            if (method) { \
                METHOD_NAME##_ptr = method->Cast<RETURN_TYPE, __VA_ARGS__>(); \
            } \
            METHOD_NAME##_initialized = true; \
        } \
        return METHOD_NAME##_ptr; \
    }

#define UNITY_STATIC_METHOD(RETURN_TYPE, METHOD_NAME, ...) \
private: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_static_ptr{}; \
    inline static bool METHOD_NAME##_static_initialized = false; \
public: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME() { \
        if (!METHOD_NAME##_static_initialized) { \
            auto method = app::getMethod(getClass(), #METHOD_NAME); \
            if (method && method->static_function) { \
                METHOD_NAME##_static_ptr = method->Cast<RETURN_TYPE, __VA_ARGS__>(); \
            } \
            METHOD_NAME##_static_initialized = true; \
        } \
        return METHOD_NAME##_static_ptr; \
    }

#define UNITY_GENERIC_STATIC_METHOD(RETURN_TYPE, METHOD_NAME, ...) \
private: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_static_ptr{}; \
    inline static bool METHOD_NAME##_static_initialized = false; \
public: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME() { \
        if (!METHOD_NAME##_static_initialized) { \
            auto method = app::getMethod(getClass(), #METHOD_NAME); \
            if (method && method->static_function) { \
                METHOD_NAME##_static_ptr = method->Cast<RETURN_TYPE, __VA_ARGS__>(); \
            } \
            METHOD_NAME##_static_initialized = true; \
        } \
        return METHOD_NAME##_static_ptr; \
    }

// Method overload macros - these handle Il2CppInspector-style overload naming
#define UNITY_METHOD_OVERLOAD_1(RETURN_TYPE, METHOD_NAME, ...) \
private: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_1_ptr{}; \
    inline static bool METHOD_NAME##_1_initialized = false; \
public: \
    inline UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_1() { \
        if (!METHOD_NAME##_1_initialized) { \
            auto method = app::getMethodWithSignature(getClass(), #METHOD_NAME, {__VA_ARGS__}); \
            if (method) { \
                METHOD_NAME##_1_ptr = method->Cast<RETURN_TYPE, __VA_ARGS__>(); \
            } \
            METHOD_NAME##_1_initialized = true; \
        } \
        return METHOD_NAME##_1_ptr; \
    }

#define UNITY_STATIC_METHOD_OVERLOAD_1(RETURN_TYPE, METHOD_NAME, ...) \
private: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_1_static_ptr{}; \
    inline static bool METHOD_NAME##_1_static_initialized = false; \
public: \
    inline static UnityResolve::MethodPointer<RETURN_TYPE, __VA_ARGS__> METHOD_NAME##_1() { \
        if (!METHOD_NAME##_1_static_initialized) { \
            auto method = app::getMethodWithSignature(getClass(), #METHOD_NAME, {__VA_ARGS__}); \
            if (method && method->static_function) { \
                METHOD_NAME##_1_static_ptr = method->Cast<RETURN_TYPE, __VA_ARGS__>(); \
            } \
            METHOD_NAME##_1_static_initialized = true; \
        } \
        return METHOD_NAME##_1_static_ptr; \
    }

#define UNITY_PROPERTY(PROPERTY_TYPE, PROPERTY_NAME) \
    inline PROPERTY_TYPE get_##PROPERTY_NAME() { \
        auto method = getClass()->Get<UnityResolve::Method>("get_" #PROPERTY_NAME); \
        if (method) return method->Invoke<PROPERTY_TYPE>(this); \
        return PROPERTY_TYPE{}; \
    } \
    inline void set_##PROPERTY_NAME(PROPERTY_TYPE value) { \
        auto method = getClass()->Get<UnityResolve::Method>("set_" #PROPERTY_NAME); \
        if (method) method->Invoke<void>(this, value); \
    } \
    __declspec(property(get=get_##PROPERTY_NAME, put=set_##PROPERTY_NAME)) PROPERTY_TYPE PROPERTY_NAME;

#define UNITY_STATIC_PROPERTY(PROPERTY_TYPE, PROPERTY_NAME) \
    inline static PROPERTY_TYPE get_##PROPERTY_NAME() { \
        auto method = getClass()->Get<UnityResolve::Method>("get_" #PROPERTY_NAME); \
        if (method && method->static_function) return method->Invoke<PROPERTY_TYPE>(); \
        return PROPERTY_TYPE{}; \
    } \
    inline static void set_##PROPERTY_NAME(PROPERTY_TYPE value) { \
        auto method = getClass()->Get<UnityResolve::Method>("set_" #PROPERTY_NAME); \
        if (method && method->static_function) method->Invoke<void>(value); \
    }

#define UNITY_CONSTRUCTOR(...) \
private: \
    inline static UnityResolve::MethodPointer<void, __VA_ARGS__> ctor_ptr{}; \
    inline static bool ctor_initialized = false; \
public: \
    inline static UnityResolve::MethodPointer<void, __VA_ARGS__> getConstructor() { \
        if (!ctor_initialized) { \
            auto method = app::getMethod(getClass(), ".ctor"); \
            if (method) { \
                ctor_ptr = method->Cast<void, __VA_ARGS__>(); \
            } \
            ctor_initialized = true; \
        } \
        return ctor_ptr; \
    } \
    template<typename... Args> \
    static auto New(Args... args) -> decltype(this) { \
        auto ctor = getConstructor(); \
        if (ctor) { \
            auto instance = getClass()->New<decltype(this)>(); \
            ctor(instance, args...); \
            return instance; \
        } \
        return nullptr; \
    }

// Namespace helpers
#define UNITY_NAMESPACE_BEGIN(NAMESPACE) namespace NAMESPACE {
#define UNITY_NAMESPACE_END() }

// Assembly helpers
#define UNITY_ASSEMBLY_REFERENCE(NAME) \
    namespace NAME##_Assembly { \
        inline UnityResolve::Assembly* getAssembly() { \
            static UnityResolve::Assembly* assembly = nullptr; \
            if (!assembly) assembly = UnityResolve::Get(#NAME ".dll"); \
            return assembly; \
        } \
    }

)";
        out.close();
    }

    static void CreateHelperFunctions(const std::string& filepath)
    {
        std::ofstream out(filepath);
        if (!out.is_open()) return;

        out << R"(#pragma once
#include "UnityResolve.hpp"
#include <string>
#include <vector>
#include <type_traits>

namespace app {
    inline UnityResolve::Class* getClass(const std::string& assembly, const std::string& className, const std::string& nameSpace = "") {
        auto assemblyPtr = UnityResolve::Get(assembly);
        if (!assemblyPtr) return nullptr;
        
        if (nameSpace.empty()) {
            return assemblyPtr->Get(className);
        } else {
            return assemblyPtr->Get(className, nameSpace);
        }
    }
    
    inline UnityResolve::Method* getMethod(UnityResolve::Class* klass, const std::string& methodName, const std::vector<std::string>& args = {}) {
        if (!klass) return nullptr;
        return klass->Get<UnityResolve::Method>(methodName, args);
    }
    
    inline UnityResolve::Method* getMethodWithSignature(UnityResolve::Class* klass, const std::string& methodName, const std::vector<std::string>& signature) {
        if (!klass) return nullptr;
        return klass->Get<UnityResolve::Method>(methodName, signature);
    }
    
    inline UnityResolve::Field* getField(UnityResolve::Class* klass, const std::string& fieldName) {
        if (!klass) return nullptr;
        return klass->Get<UnityResolve::Field>(fieldName);
    }
    
    template<typename T>
    inline T getStaticFieldValue(UnityResolve::Class* klass, const std::string& fieldName) {
        auto field = getField(klass, fieldName);
        if (field && field->static_field) {
            T value;
            field->GetStaticValue(&value);
            return value;
        }
        return T{};
    }
    
    template<typename T>
    inline void setStaticFieldValue(UnityResolve::Class* klass, const std::string& fieldName, T value) {
        auto field = getField(klass, fieldName);
        if (field && field->static_field) {
            field->SetStaticValue(&value);
        }
    }
    
    template<typename T>
    inline std::vector<T> findObjectsOfType(UnityResolve::Class* klass) {
        if (!klass) return {};
        return klass->FindObjectsByType<T>();
    }
    
    inline UnityResolve::Method* getGenericMethod(UnityResolve::Class* klass, const std::string& methodName, const std::vector<std::string>& genericTypes, const std::vector<std::string>& args = {}) {
        // This would need additional implementation in UnityResolve for full generic support
        // For now, fallback to regular method resolution
        return getMethod(klass, methodName, args);
    }
    
    inline UnityResolve::Method* getPropertyGetter(UnityResolve::Class* klass, const std::string& propertyName) {
        return getMethod(klass, "get_" + propertyName);
    }
    
    inline UnityResolve::Method* getPropertySetter(UnityResolve::Class* klass, const std::string& propertyName) {
        return getMethod(klass, "set_" + propertyName);
    }
    
    template<typename... Args>
    inline UnityResolve::Method* getMethodOverload(UnityResolve::Class* klass, const std::string& methodName, int overloadIndex = 0) {
        if (!klass) return nullptr;
        
        std::vector<UnityResolve::Method*> overloads;
        for (auto method : klass->methods) {
            if (method->name == methodName && method->args.size() == sizeof...(Args)) {
                overloads.push_back(method);
            }
        }
        
        if (overloadIndex < overloads.size()) {
            return overloads[overloadIndex];
        }
        
        return nullptr;
    }
}

// Utility macros for common operations
#define GET_UNITY_CLASS(assembly, className) app::getClass(assembly, className)
#define GET_UNITY_METHOD(klass, methodName) app::getMethod(klass, methodName)
#define GET_UNITY_FIELD(klass, fieldName) app::getField(klass, fieldName)
#define FIND_OBJECTS_OF_TYPE(type, klass) app::findObjectsOfType<type>(klass)

// Type-safe casting helpers
#define UNITY_CAST(obj, TargetType) \
    (static_cast<TargetType*>(obj))

#define UNITY_AS(obj, TargetType) UNITY_CAST(obj, TargetType)

// Memory validation helpers
#define UNITY_VALIDATE_OFFSET(ClassName, FieldName, ExpectedOffset) \
    static_assert(offsetof(ClassName, FieldName) == ExpectedOffset, \
                  "Field offset mismatch: " #FieldName " expected at " #ExpectedOffset)

#define UNITY_VALIDATE_SIZE(ClassName, ExpectedSize) \
    static_assert(sizeof(ClassName) <= ExpectedSize, \
                  "Class size exceeds expected Unity size: " #ExpectedSize)

// Runtime debugging helpers
#define UNITY_DEBUG_CLASS_INFO(klass) \
    do { \
        if (klass) { \
            printf("Class: %s\n", klass->name.c_str()); \
            printf("Namespace: %s\n", klass->namespaze.c_str()); \
            printf("Parent: %s\n", klass->parent.c_str()); \
            printf("Fields: %zu\n", klass->fields.size()); \
            printf("Methods: %zu\n", klass->methods.size()); \
        } \
    } while(0)

// Enhanced error handling
#define UNITY_TRY_INVOKE(method_call) \
    [&]() { \
        try { \
            return method_call; \
        } catch (const std::exception& e) { \
            printf("Unity method invocation failed: %s\n", e.what()); \
            return decltype(method_call){}; \
        } \
    }()

// Overload helpers
#define UNITY_GET_METHOD_OVERLOAD(klass, methodName, overloadIndex) \
    app::getMethodOverload(klass, methodName, overloadIndex)

// String literal helpers
#define UNITY_GET_STRING_LITERAL(klass, fieldName) \
    [&]() { \
        auto field = app::getField(klass, fieldName); \
        if (field && field->static_field) { \
            UnityResolve::UnityType::String* str; \
            field->GetStaticValue(&str); \
            return str ? str->ToString() : std::string(""); \
        } \
        return std::string(""); \
    }()

)";
        out.close();
    }
};

static void DumpToCppHeaders(const std::string& outputPath)
{
    UnityCppDumper::DumpToCppHeaders(outputPath);
    LOG_INFO("Output located in \"{}\"", outputPath);
}
