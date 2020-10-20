#include "gmock/gmock.h"

#include "EngineServer.h"
#include "SessionManager.h"
#include "IParser.h"
#include "IEngine.h"
#include "IUser.h"
#include "IGameEngine.h"

using ::testing::InSequence;

class MockClientSocket: public ClientSocket {
public:
    MockClientSocket(int socket, bool option = false): ClientSocket(socket, option){}
    MOCK_METHOD(int, Recieve, (std::string& msg));
    MOCK_METHOD(int, SendMsg, (const std::string& msg));
};

class MockUser: public User {
public:
    MockUser(std::shared_ptr<ClientSocket> sock): User(sock){}
    MOCK_METHOD(void, write, (const std::string& msg));
};


Map CreateTestMap(bool num) {
    std::string coordinates;
    if (num) {
        coordinates = "0 0 0 3\n3 3 3 3\n4 0 5 0\n9 0 9 1\n6 3 6 3\n3 5 3 5\n8 4 8 4\n0 9 2 9\n5 6 6 6\n9 7 9 9\n";
    } else {
        coordinates = "0 0 0 3\n3 3 3 3\n4 1 5 1\n9 0 9 1\n2 6 2 6\n0 5 0 5\n8 4 8 4\n0 9 2 9\n5 6 6 6\n9 6 9 8\n";
    }

    std::stringstream stream(coordinates);
    Map map;

    for (int i = 0; i < 10; i++) {
        size_t x_s = 0;
        size_t y_s = 0;
        stream >> x_s >> y_s;
        size_t x_e = 0;
        size_t y_e = 0;
        stream >> x_e >> y_e;
        Ship ship(x_s, y_s, x_e, y_e);
        map.ships.insert({i, ship});
    }
    return map;
}

TEST(GameTest, InsertMap) {
    std::shared_ptr<IGameEngine> GameEngine = std::make_shared<IGameEngine>();

    Map map = CreateTestMap(true);
    Map map2 = CreateTestMap(false);

    EXPECT_TRUE(GameEngine->InsertMap(1, map));
    EXPECT_TRUE(GameEngine->InsertMap(2, map2));
    GameEngine->StartGame();
    GameEngine->SetStep(1);
    std::shared_ptr<GameState> gs = std::make_shared<GameState>();

    std::shared_ptr<GameState> game = GameEngine->UpdateGame(1, {0, 0});
    EXPECT_EQ(1, game->next_step_id_);
    EXPECT_EQ(Result::Hit, game->result_);

    std::shared_ptr<GameState> g = GameEngine->UpdateGame(1, {1, 0});
    EXPECT_EQ(2, g->next_step_id_);
    EXPECT_EQ(Result::Miss, g->result_);

    g = GameEngine->UpdateGame(1, {2, 1});
    EXPECT_EQ(nullptr, g);

    g = GameEngine->UpdateGame(2, {3, 3});
    EXPECT_EQ(2, g->next_step_id_);
    EXPECT_EQ(Result::Kill, g->result_);

    EXPECT_EQ(3, g->killed_ship_.start_.x_);
    EXPECT_EQ(3, g->killed_ship_.start_.y_);
    EXPECT_EQ(3, g->killed_ship_.end_.x_);
    EXPECT_EQ(3, g->killed_ship_.end_.y_);

    g = GameEngine->UpdateGame(2, {3, 3});
    EXPECT_EQ(1, g->next_step_id_);
    EXPECT_EQ(Result::Miss, g->result_);
    int winner_id = -1;

    GameEngine->EndGame(1, winner_id);
    EXPECT_EQ(2, winner_id);

}

TEST(SessionManagerTest, CreateJoinSessionTest) {
    std::shared_ptr<ClientSocket> cl = std::make_shared<MockClientSocket>(5);
    std::shared_ptr<ClientSocket> cl1 = std::make_shared<MockClientSocket>(7);
    std::shared_ptr<ClientSocket> cl2 = std::make_shared<MockClientSocket>(6);
    std::shared_ptr<ClientSocket> cl3 = std::make_shared<MockClientSocket>(8);

    UserPtr user = std::make_shared<MockUser>(cl);
    UserPtr user1 = std::make_shared<MockUser>(cl1);
    UserPtr user2 = std::make_shared<MockUser>(cl2);
    UserPtr user3 = std::make_shared<MockUser>(cl3);

    SessionManager session_manager;

    EXPECT_TRUE(session_manager.CreateSession(user));
    EXPECT_EQ(Error::Success, session_manager.AddUserInSession(user2, 1));

    EXPECT_EQ(Error::Full, session_manager.AddUserInSession(user1, 1));
    EXPECT_EQ(Error::NotFound, session_manager.AddUserInSession(user1, 2));

    EXPECT_TRUE(session_manager.CreateSession(user1));
    EXPECT_EQ(Error::UserExist, session_manager.AddUserInSession(user1, 2));

    std::shared_ptr<EraseState> es = std::make_shared<EraseState>();
    EXPECT_EQ(es->winner_id_, session_manager.EraseUser(user, 1)->winner_id_);

    EXPECT_EQ(nullptr, session_manager.EraseUser(user, 3));

    EXPECT_EQ(Error::Success, session_manager.AddUserInSession(user3, 1));
}



TEST(MapTest, InsertPoints) {
    Map map = CreateTestMap(true);
    GameMap g_map;
    IGameEngine engine;
    std::shared_ptr<GameMap> gm = engine.ValidateMap(map);

    int killed_ship = -1;

    EXPECT_EQ(Result::Hit, gm->InsertPoint({0, 0}, killed_ship));
    EXPECT_EQ(-1, killed_ship);

    EXPECT_EQ(Result::Kill, gm->InsertPoint({3, 3}, killed_ship));
    EXPECT_EQ(1, killed_ship);

    EXPECT_EQ(Result::Miss, gm->InsertPoint({2, 2}, killed_ship));
    EXPECT_EQ(9, gm->Count());

    EXPECT_EQ(Result::Miss, gm->InsertPoint({0, 0}, killed_ship));
}

TEST(SessionTest, SessionGameTest) {
    std::shared_ptr<ClientSocket> cl = std::make_shared<MockClientSocket>(5);
    std::shared_ptr<ClientSocket> cl1 = std::make_shared<MockClientSocket>(7);
    std::shared_ptr<ClientSocket> cl2 = std::make_shared<MockClientSocket>(6);
    std::shared_ptr<ClientSocket> cl3 = std::make_shared<MockClientSocket>(8);

    UserPtr user = std::make_shared<MockUser>(cl);
    UserPtr user1 = std::make_shared<MockUser>(cl1);
    UserPtr user2 = std::make_shared<MockUser>(cl2);
    UserPtr user3 = std::make_shared<MockUser>(cl3);

    Session session(user);
    EXPECT_EQ(Error::Success, session.AddUserInSession(user1));

    Map map = CreateTestMap(true);
    Map map2 = CreateTestMap(false);

    EXPECT_EQ(Error::Wait, session.StartGame(user, map));
    EXPECT_EQ(Error::Started, session.StartGame(user1, map2));
    int winner_id = user1->GetId();
    EraseState t_es(winner_id);
    EraseState es = session.EraseUser(user);

    EXPECT_EQ(es.winner_id_, t_es.winner_id_);
    EXPECT_EQ(es.started_, t_es.started_);

    EXPECT_EQ(Error::Success, session.AddUserInSession(user2));
    EXPECT_EQ(Error::Full, session.AddUserInSession(user3));

    EXPECT_EQ(Error::Wait, session.StartGame(user2, map));
    EXPECT_EQ(Error::Started, session.StartGame(user1, map2));

    winner_id = user1->GetId();
    t_es = EraseState(winner_id);
    es = session.EraseUser(user2);

    EXPECT_EQ(es.winner_id_, t_es.winner_id_);
    EXPECT_EQ(es.started_, t_es.started_);
}

TEST(TestSessionManager, Stress) {
    std::vector<UserPtr> test_users;
    SessionManager session_manager;
    for (int i = 0; i < 1000; i++) {
        std::shared_ptr<ClientSocket> cl = std::make_shared<MockClientSocket>(i);
        UserPtr user = std::make_shared<MockUser>(cl);
        if (i % 2 == 0) {
            EXPECT_EQ(4 + (i + 1) / 2, session_manager.CreateSession(user));
        } else {
            int num = 3 + (i + 1) / 2;
            EXPECT_EQ(Error::Success, session_manager.AddUserInSession(user, num));
        }
        test_users.push_back(user);
    }
}



TEST(TestParser, Deserialize) {
    Map map = CreateTestMap(true);

    DataRequest rq;
    rq.data_ = AuthData("grisha", "og");
    rq.type_ = Route::Enter;
    rq.session_id_ = 1;
    rq.user_id_ = 1;
    rq.map_ = map;
    rq.point_ = {2, 3};

    std::string x;
    std::stringstream ss(x);
    msgpack::pack(ss, rq);

    Parser parser;

    std::shared_ptr<DataRequest> rq1 = parser.Deserialize(ss.str());

    EXPECT_EQ(rq1->type_, rq.type_);
    EXPECT_EQ(rq1->data_.login_, rq.data_.login_);
    EXPECT_EQ(rq1->data_.password_, rq.data_.password_);
    EXPECT_EQ(rq1->user_id_, rq.user_id_);
    EXPECT_EQ(rq1->point_.x_, rq.point_.x_);
    EXPECT_EQ(rq1->point_.y_, rq.point_.y_);

    for (auto m: rq.map_.ships) {
        Ship sh = rq1->map_.ships.find(m.first)->second;
        EXPECT_EQ(sh.end_.x_, m.second.end_.x_);
        EXPECT_EQ(sh.end_.y_, m.second.end_.y_);
        EXPECT_EQ(sh.start_.x_, m.second.start_.x_);
        EXPECT_EQ(sh.start_.y_, m.second.start_.y_);
    }
}

TEST(TestParser, Serialize) {
    std::shared_ptr<DataResponse> rp = std::make_shared<DataResponse>();

    rp->type_ = Route::Enter;
    rp->session_id_ = 1;
    rp->user_id_ = 1;
    rp->point_ = {2, 3};
    rp->erase_state_ = EraseState(2);

    Ship ship = Ship({3, 3}, {3, 2});

    rp->game_state_ = GameState(3, Result::Hit, ship);

    std::string response;
    Parser parser;

    parser.Serialize(rp, response);

    std::shared_ptr<DataResponse> rp1 = std::make_shared<DataResponse>();
    msgpack::object_handle oh = msgpack::unpack(response.data(), response.size());
    msgpack::object obj = oh.get();
    obj.convert(*rp1);

    EXPECT_EQ(rp->type_, rp1->type_);
    EXPECT_EQ(rp->session_id_, rp1->session_id_);
    EXPECT_EQ(rp->point_.x_, rp1->point_.x_);
    EXPECT_EQ(rp->point_.y_, rp1->point_.y_);
    EXPECT_EQ(rp->erase_state_.started_, rp1->erase_state_.started_);
    EXPECT_EQ(rp->erase_state_.winner_id_, rp1->erase_state_.winner_id_);
    EXPECT_EQ(rp->game_state_.result_, rp1->game_state_.result_);
    EXPECT_EQ(rp->game_state_.next_step_id_, rp1->game_state_.next_step_id_);

    EXPECT_EQ(rp->game_state_.killed_ship_.start_.x_, rp->game_state_.killed_ship_.start_.x_);
    EXPECT_EQ(rp->game_state_.killed_ship_.start_.y_, rp->game_state_.killed_ship_.start_.y_);
    EXPECT_EQ(rp->game_state_.killed_ship_.end_.x_, rp->game_state_.killed_ship_.end_.x_);
    EXPECT_EQ(rp->game_state_.killed_ship_.end_.y_, rp->game_state_.killed_ship_.end_.y_);

}

TEST(DBAccess, TestObj){
    DBAccess obj;

    obj.AddUser("user", "pswrd");
    EXPECT_TRUE(obj.CheckUser("user", "pswrd"));

    EXPECT_FALSE(obj.CheckUser("user", "password"));
    EXPECT_FALSE(obj.CheckUser("usr", "pass"));
    EXPECT_FALSE(obj.AddUser("user", "password"));
}
