// cpp_etfer.cpp : Defines the entry point for the application.
//

#include <CppEtfer/CppEtfer.hpp>
#include <jsonifier/Index.hpp>
#include <unordered_set>
#include <iostream>
#include <map>

struct User {
	std::string discriminator{};
	std::string global_name{};
	std::string username{};
	std::string avatar{};
	std::string email{};
	bool mfa_enabled{};
	bool verified{};
	uint64_t id{};
	int flags{};
	bool bot{};
};

struct Guild {
	bool unavailable{};
	std::string id{};
};

struct Session {
	std::string resume_gateway_url{};
	std::vector<Guild> guilds{};
	std::string session_type{};
	std::string session_id{};
};

struct Data {
	std::map<std::string, std::string> user_settings{};
	std::string shard{};
	std::string _trace{};
	Session session{};
	User user{};
	int v{};
};

struct ReadyData {
	std::string t{};
	int op{};
	Data d{};
	int s{};
};

template<> struct cpp_etfer::core<User> {
	using ValueType					 = User;
	static constexpr auto parseValue = createObject("verified", &ValueType::verified, "username", &ValueType::username, "mfa_enabled", &ValueType::mfa_enabled, "id",
		&ValueType::id, "global_name", &ValueType::global_name, "flags", &ValueType::flags, "email", &ValueType::email, "discriminator", &ValueType::discriminator, "bot",
		&ValueType::bot, "avatar", &ValueType::avatar);
};

template<> struct cpp_etfer::core<Guild> {
	using ValueType					 = Guild;
	static constexpr auto parseValue = createObject("unavailable", &ValueType::unavailable, "id", &ValueType::id);
};

template<> struct cpp_etfer::core<Session> {
	using ValueType					 = Session;
	static constexpr auto parseValue = createObject("session_type", &ValueType::session_type, "session_id", &ValueType::session_id, "resume_gateway_url",
		&ValueType::resume_gateway_url, "guilds", &ValueType::guilds);
};

template<> struct cpp_etfer::core<Data> {
	using ValueType					 = Data;
	static constexpr auto parseValue =
		createObject("session", &ValueType::session, "v", &ValueType::v, "user", &ValueType::user, "shard", &ValueType::shard);
};

template<> struct cpp_etfer::core<ReadyData> {
	using ValueType					 = ReadyData;
	static constexpr auto parseValue = createObject("t", &ValueType::t, "s", &ValueType::s, "op", &ValueType::op, "d", &ValueType::d);
};

/// @brief Activity types.
enum class ActivityType : uint8_t {
	Game	  = 0,///< Game.
	Streaming = 1,///< Streaming.
	Listening = 2,///< Listening.
	Watching  = 3,///< Watching.
	Custom	  = 4,///< Custom.
	Competing = 5///< Competing.
};

/// @brief Activity data.
struct ActivityData {
	std::string applicationId{};///< Application ID for the game.
	std::string created_at{};///< Unix timestamp(in milliseconds) of when the activity was added to the user's session.
	std::string details{};///< What the player is currently doing.
	ActivityType type{};///< Activity's type.
	std::string state{};///< User's current party status, or text used for a custom status.
	std::string name{};///< Name of the activity.
	std::string url{};///< Stream URL, is validated when type is 1.

	operator cpp_etfer::etf_serializer() {
		cpp_etfer::etf_serializer serializer{};
		serializer["application_id"] = applicationId;
		serializer["created_at"]	 = created_at;
		serializer["details"]		 = details;
		serializer["state"]			 = state;
		serializer["type"]			 = type;
		serializer["name"]			 = name;
		serializer["url"]			 = url;
		return serializer;
	}
};

struct TestStruct {
	int32_t testInt{};
};

enum class PresenceUpdateState { Online = 0, Do_Not_Disturb = 1, Idle = 2, Invisible = 3, Offline = 4 };

/// @brief For updating a User's presence.
struct UpdatePresenceData {
	std::unordered_set<std::string> excludedKeys{};
	std::vector<ActivityData> activities{};
	PresenceUpdateState status{};
	std::string statusReal{};
	int64_t since{};
	bool afk{};

	operator cpp_etfer::etf_serializer() {
		cpp_etfer::etf_serializer data{};
		for (auto& value: activities) {
			cpp_etfer::etf_serializer newData{};
			if (value.url != "") {
				newData["url"] = std::string_view{ value.url };
			}
			newData["name"] = std::string_view{ value.name };
			newData["type"] = value.type;
			newData["state"] = value.state;
			data["activities"].emplaceBack(newData);
		} 
		
		switch (status) {
			case PresenceUpdateState::Online: {
				data["status"] = statusReal;
				break;
			}
			case PresenceUpdateState::Idle: {
				data["status"] = statusReal;
				break;
			}
			case PresenceUpdateState::Invisible: {
				data["status"] = statusReal;
				break;
			}
			case PresenceUpdateState::Do_Not_Disturb: {
				data["status"] = statusReal;
				break;
			}
			case PresenceUpdateState::Offline: {
				data["status"] = statusReal;
				break;
			}
		}
		data["since"] = since;
		data["afk"]	  = false;
		return data;
	}

  protected:
	
};

template<> struct cpp_etfer::core<ActivityData> {
	using value_type				 = ActivityData;
	static constexpr auto parseValue = createObject("name", &value_type::name, "type", &value_type::type, "state", &value_type::state);
};

template<> struct cpp_etfer::core<UpdatePresenceData> {
	using value_type = UpdatePresenceData;
	static constexpr auto parseValue = createObject("afk", &value_type::afk, "activities", &value_type::activities, "since", &value_type::since, "status", &value_type::statusReal);
};

int main()
{
	std::vector<uint8_t> stringValues = { 131, 116, 0, 0, 0, 4, 100, 0, 1, 100, 116, 0, 0, 0, 16, 100, 0, 6, 95, 116, 114, 97, 99, 101, 108, 0, 0, 0, 1, 109, 0, 0, 3, 196, 91, 34,
		103, 97, 116, 101, 119, 97, 121, 45, 112, 114, 100, 45, 117, 115, 45, 101, 97, 115, 116, 49, 45, 98, 45, 99, 104, 49, 118, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34,
		58, 49, 56, 51, 50, 51, 52, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 34, 105, 100, 95, 99, 114, 101, 97, 116, 101, 100, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34,
		58, 49, 48, 51, 51, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 115, 101, 115, 115, 105, 111, 110, 95, 108, 111, 111, 107, 117, 112, 95, 116, 105, 109, 101,
		34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 51, 53, 55, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 115, 101, 115, 115, 105, 111, 110, 95, 108,
		111, 111, 107, 117, 112, 95, 102, 105, 110, 105, 115, 104, 101, 100, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 49, 54, 44, 34, 99, 97, 108, 108, 115, 34, 58,
		91, 93, 125, 44, 34, 100, 105, 115, 99, 111, 114, 100, 45, 115, 101, 115, 115, 105, 111, 110, 115, 45, 112, 114, 100, 45, 50, 45, 50, 53, 34, 44, 123, 34, 109, 105, 99,
		114, 111, 115, 34, 58, 49, 55, 57, 56, 56, 51, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 34, 115, 116, 97, 114, 116, 95, 115, 101, 115, 115, 105, 111, 110, 34, 44, 123,
		34, 109, 105, 99, 114, 111, 115, 34, 58, 56, 54, 57, 52, 54, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 34, 100, 105, 115, 99, 111, 114, 100, 45, 97, 112, 105, 45, 53, 100,
		99, 100, 102, 55, 98, 99, 52, 56, 45, 114, 102, 103, 50, 115, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 55, 57, 57, 57, 55, 44, 34, 99, 97, 108, 108, 115, 34,
		58, 91, 34, 103, 101, 116, 95, 117, 115, 101, 114, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 50, 49, 51, 54, 57, 125, 44, 34, 103, 101, 116, 95, 103, 117, 105,
		108, 100, 115, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 56, 52, 56, 49, 125, 44, 34, 115, 101, 110, 100, 95, 115, 99, 104, 101, 100, 117, 108, 101, 100, 95,
		100, 101, 108, 101, 116, 105, 111, 110, 95, 109, 101, 115, 115, 97, 103, 101, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 49, 53, 125, 44, 34, 103, 117, 105, 108,
		100, 95, 106, 111, 105, 110, 95, 114, 101, 113, 117, 101, 115, 116, 115, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 57, 49, 49, 125, 44, 34, 97, 117, 116, 104,
		111, 114, 105, 122, 101, 100, 95, 105, 112, 95, 99, 111, 114, 111, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 49, 53, 125, 93, 125, 93, 125, 44, 34, 115, 116,
		97, 114, 116, 105, 110, 103, 95, 103, 117, 105, 108, 100, 95, 99, 111, 110, 110, 101, 99, 116, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 50, 51, 56, 44, 34, 99,
		97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 112, 114, 101, 115, 101, 110, 99, 101, 95, 115, 116, 97, 114, 116, 101, 100, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115,
		34, 58, 52, 52, 54, 57, 50, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 103, 117, 105, 108, 100, 115, 95, 115, 116, 97, 114, 116, 101, 100, 34, 44, 123, 34,
		109, 105, 99, 114, 111, 115, 34, 58, 49, 55, 48, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 103, 117, 105, 108, 100, 115, 95, 99, 111, 110, 110, 101, 99,
		116, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 50, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 112, 114, 101, 115, 101, 110, 99, 101, 95, 99,
		111, 110, 110, 101, 99, 116, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 52, 55, 56, 48, 57, 44, 34, 99, 97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 99, 111,
		110, 110, 101, 99, 116, 95, 102, 105, 110, 105, 115, 104, 101, 100, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 52, 55, 56, 49, 52, 44, 34, 99, 97, 108, 108, 115,
		34, 58, 91, 93, 125, 44, 34, 98, 117, 105, 108, 100, 95, 114, 101, 97, 100, 121, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 50, 48, 44, 34, 99, 97, 108, 108,
		115, 34, 58, 91, 93, 125, 44, 34, 99, 108, 101, 97, 110, 95, 114, 101, 97, 100, 121, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 48, 44, 34, 99, 97, 108, 108,
		115, 34, 58, 91, 93, 125, 44, 34, 111, 112, 116, 105, 109, 105, 122, 101, 95, 114, 101, 97, 100, 121, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 49, 44, 34, 99,
		97, 108, 108, 115, 34, 58, 91, 93, 125, 44, 34, 115, 112, 108, 105, 116, 95, 114, 101, 97, 100, 121, 34, 44, 123, 34, 109, 105, 99, 114, 111, 115, 34, 58, 48, 44, 34, 99,
		97, 108, 108, 115, 34, 58, 91, 93, 125, 93, 125, 93, 125, 93, 106, 100, 0, 11, 97, 112, 112, 108, 105, 99, 97, 116, 105, 111, 110, 116, 0, 0, 0, 2, 100, 0, 5, 102, 108, 97,
		103, 115, 98, 1, 168, 160, 0, 100, 0, 2, 105, 100, 110, 8, 0, 116, 48, 132, 118, 50, 206, 219, 15, 100, 0, 4, 97, 117, 116, 104, 116, 0, 0, 0, 0, 100, 0, 23, 103, 101, 111,
		95, 111, 114, 100, 101, 114, 101, 100, 95, 114, 116, 99, 95, 114, 101, 103, 105, 111, 110, 115, 108, 0, 0, 0, 5, 109, 0, 0, 0, 6, 110, 101, 119, 97, 114, 107, 109, 0, 0, 0,
		7, 117, 115, 45, 101, 97, 115, 116, 109, 0, 0, 0, 10, 117, 115, 45, 99, 101, 110, 116, 114, 97, 108, 109, 0, 0, 0, 7, 97, 116, 108, 97, 110, 116, 97, 109, 0, 0, 0, 8, 117,
		115, 45, 115, 111, 117, 116, 104, 106, 100, 0, 19, 103, 117, 105, 108, 100, 95, 106, 111, 105, 110, 95, 114, 101, 113, 117, 101, 115, 116, 115, 106, 100, 0, 6, 103, 117,
		105, 108, 100, 115, 108, 0, 0, 0, 7, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 10, 128, 66, 127, 38, 218, 237, 12, 100, 0, 11, 117, 110, 97, 118, 97, 105, 108, 97,
		98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 10, 48, 2, 232, 100, 212, 192, 13, 100, 0, 11, 117, 110, 97, 118, 97, 105,
		108, 97, 98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 71, 0, 196, 181, 192, 31, 207, 13, 100, 0, 11, 117, 110, 97, 118, 97,
		105, 108, 97, 98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 10, 0, 132, 73, 247, 79, 48, 14, 100, 0, 11, 117, 110, 97, 118,
		97, 105, 108, 97, 98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 30, 0, 68, 93, 95, 47, 85, 14, 100, 0, 11, 117, 110, 97,
		118, 97, 105, 108, 97, 98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 58, 16, 196, 98, 103, 155, 247, 14, 100, 0, 11, 117,
		110, 97, 118, 97, 105, 108, 97, 98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 116, 0, 0, 0, 2, 100, 0, 2, 105, 100, 110, 8, 0, 10, 64, 132, 71, 0, 39, 181, 15, 100, 0, 11,
		117, 110, 97, 118, 97, 105, 108, 97, 98, 108, 101, 100, 0, 4, 116, 114, 117, 101, 106, 100, 0, 9, 112, 114, 101, 115, 101, 110, 99, 101, 115, 106, 100, 0, 16, 112, 114,
		105, 118, 97, 116, 101, 95, 99, 104, 97, 110, 110, 101, 108, 115, 106, 100, 0, 13, 114, 101, 108, 97, 116, 105, 111, 110, 115, 104, 105, 112, 115, 106, 100, 0, 18, 114,
		101, 115, 117, 109, 101, 95, 103, 97, 116, 101, 119, 97, 121, 95, 117, 114, 108, 109, 0, 0, 0, 35, 119, 115, 115, 58, 47, 47, 103, 97, 116, 101, 119, 97, 121, 45, 117, 115,
		45, 101, 97, 115, 116, 49, 45, 98, 46, 100, 105, 115, 99, 111, 114, 100, 46, 103, 103, 100, 0, 10, 115, 101, 115, 115, 105, 111, 110, 95, 105, 100, 109, 0, 0, 0, 32, 48,
		53, 101, 56, 50, 50, 98, 49, 55, 101, 51, 48, 98, 101, 98, 101, 97, 55, 51, 48, 102, 51, 52, 56, 51, 57, 51, 55, 50, 100, 57, 55, 100, 0, 12, 115, 101, 115, 115, 105, 111,
		110, 95, 116, 121, 112, 101, 100, 0, 6, 110, 111, 114, 109, 97, 108, 100, 0, 5, 115, 104, 97, 114, 100, 107, 0, 2, 0, 1, 100, 0, 4, 117, 115, 101, 114, 116, 0, 0, 0, 10,
		100, 0, 6, 97, 118, 97, 116, 97, 114, 109, 0, 0, 0, 32, 56, 56, 98, 100, 57, 99, 101, 55, 98, 102, 56, 56, 57, 99, 48, 100, 51, 54, 102, 98, 52, 97, 102, 100, 51, 55, 50,
		53, 57, 48, 48, 98, 100, 0, 3, 98, 111, 116, 100, 0, 4, 116, 114, 117, 101, 100, 0, 13, 100, 105, 115, 99, 114, 105, 109, 105, 110, 97, 116, 111, 114, 109, 0, 0, 0, 4, 51,
		48, 53, 53, 100, 0, 5, 101, 109, 97, 105, 108, 100, 0, 3, 110, 105, 108, 100, 0, 5, 102, 108, 97, 103, 115, 97, 0, 100, 0, 11, 103, 108, 111, 98, 97, 108, 95, 110, 97, 109,
		101, 100, 0, 3, 110, 105, 108, 100, 0, 2, 105, 100, 110, 8, 0, 116, 48, 132, 118, 50, 206, 219, 15, 100, 0, 11, 109, 102, 97, 95, 101, 110, 97, 98, 108, 101, 100, 100, 0,
		5, 102, 97, 108, 115, 101, 100, 0, 8, 117, 115, 101, 114, 110, 97, 109, 101, 109, 0, 0, 0, 17, 77, 66, 111, 116, 45, 77, 117, 115, 105, 99, 72, 111, 117, 115, 101, 45, 50,
		100, 0, 8, 118, 101, 114, 105, 102, 105, 101, 100, 100, 0, 4, 116, 114, 117, 101, 100, 0, 13, 117, 115, 101, 114, 95, 115, 101, 116, 116, 105, 110, 103, 115, 116, 0, 0, 0,
		0, 100, 0, 1, 118, 97, 10, 100, 0, 2, 111, 112, 97, 0, 100, 0, 1, 115, 97, 1, 100, 0, 1, 116, 100, 0, 5, 82, 69, 65, 68, 89 };

	std::basic_string<uint8_t> presenceUpdateString{};

	for (uint8_t value: stringValues) {
		presenceUpdateString.push_back(value);
	}
	try {
		UpdatePresenceData updatePresenceData{};
		updatePresenceData.afk	  = true;
		//updatePresenceData.status = PresenceUpdateState::Idle;
		ActivityData activityData{};
		activityData.name = "Test_Activity_01";
		activityData.type = ActivityType::Competing;
		activityData.state = "TEST_STATE";
		updatePresenceData.activities.emplace_back(activityData);
		activityData.name = "Test_Activity_02";
		activityData.type = ActivityType::Custom;
		updatePresenceData.activities.emplace_back(activityData);
		ReadyData dataNew{};

		auto newString = updatePresenceData.operator cpp_etfer::etf_serializer().operator std::basic_string<uint8_t>();

		cpp_etfer::etf_parser parser{};
		updatePresenceData.activities.clear();
		parser.parseEtfToData(updatePresenceData, newString);
		std::cout << "Json data: " << updatePresenceData.activities[0].name << std::endl;
		std::cout << "Json data: " << ( int32_t )updatePresenceData.activities[0].type << std::endl;
		std::cout << "Json data: " << updatePresenceData.activities[0].state << std::endl;
		std::cout << "Json data: " << parser.parseEtfToJson(newString) << std::endl;

	} catch (std::runtime_error& error) {
		std::cout << "Error: " << error.what() << std::endl;
	}

	
	return 0;
}
