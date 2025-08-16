# UnityCppDumper
This project is still **WIP**. It's a runtime dumper that was initially designed for [BA-Cheeto](https://github.com/Taiga74164/BA-Cheeto) and is supposed to be used along side [UnityResolve](https://github.com/issuimo/UnityResolve.hpp)

##  TODO:
- Handle generics properly (Params, Class, etc.)
- Inheritance
- Fixes for mono

## Usage:
1. Compile Release build
2. Inject to Unity game
3. Run the game
4. Dump will be saved to `{game_directory}/.dump/` 

## Example dump:
``` c++
// Assembly: BlueArchive.dll
// Namespace: FlatData
// Class Flags: 0x101
// Instance Size: 20
enum class TacticEntityType_Enum : int32_t {
    None = 0,
    Student = 1,
    Minion = 2,
    Elite = 4,
    Champion = 8,
    Boss = 16,
    Obstacle = 32,
    Servant = 64,
    Vehicle = 128,
    Summoned = 256,
    Hallucination = 512,
    DestructibleProjectile = 1024,
};

// Assembly: BlueArchive.dll
// Namespace: MX.Logic.Skills
// Class Flags: 0x2109 (Public, Sealed, Serializable)
// Instance Size: 32 bytes
// Is Abstract: false
// Is Interface: false
class EssentialCandidateRule final : public ValueType {
    UNITY_CLASS_DECL("BlueArchive.dll", "EssentialCandidateRule")

public:
    // Fields
    // Offset: 0x10, Flags: 0x6, Type: MX.Logic.Skills.TargetSideId
    UNITY_FIELD(TargetSideId_Enum, TargetSide, 0x10)
    // Offset: 0x14, Flags: 0x6, Type: MX.Logic.Skills.TargetingType
    UNITY_FIELD(TargetingType_Enum, TargetingType, 0x14)
    // Offset: 0x18, Flags: 0x6, Type: MX.Logic.Skills.TargetEntityType
    UNITY_FIELD(TargetEntityType_Enum, ApplyEntityType, 0x18)
    // Offset: 0x1c, Flags: 0x6, Type: System.Int32
    UNITY_FIELD(int32_t, MaxTargetCount, 0x1c)

public:
    // Methods
    // Flags: 0xc6, RVA: 0x1027600, ParamCount: 0, ReturnType: System.Int32 (Virtual)
    UNITY_METHOD(int32_t, GetHashCode)
    // Flags: 0xc6, RVA: 0x1027690, ParamCount: 1, ReturnType: System.Boolean (Virtual)
    // Parameters: UnityResolve::UnityType::Object* obj
    UNITY_METHOD(bool, Equals_1, UnityResolve::UnityType::Object*)
    // Flags: 0x1e6, RVA: 0x10271b0, ParamCount: 1, ReturnType: System.Boolean (Virtual, Final)
    // Parameters: EssentialCandidateRule other
    UNITY_METHOD(bool, Equals_2, EssentialCandidateRule)
    // Flags: 0x886, RVA: 0x1027840, ParamCount: 0, ReturnType: System.Boolean
    UNITY_METHOD(bool, get_IsValid)
    // Flags: 0x5c6, ParamCount: 1, ReturnType: System.Boolean (Virtual, Abstract)
    // Parameters: EssentialCandidateRule other
    UNITY_METHOD(bool, Equals_3, EssentialCandidateRule)

};
```

