#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include <QObject>
#include "data/Player.h"
#include "data/Team.h"
#include "data/Card.h"

class DataInterface : public QObject
{
    Q_OBJECT
public:
    explicit DataInterface(QObject *parent = 0);    
    void initCardDB();
    void initPlayerList(QString msg);
    void initTeam(int moraleMax);
    void sortPlayers();
    void addHandCard(Card* card);
    void removeHandCard(Card *card);
    void setMyself(Player* player);
    void setID(int id);
    void setPlayerMax(int playerMax);

    Card* getCard(int id);
    int getID();
    int getFirstPlayerID(){return firstPlayerID;}
    Player* getMyself();
    Team* getMyTeam(){return myTeam;}
    Team* getOtherTeam(){return otherTeam;}
    int getPlayerMax();
    QList<Card*> getHandCards();
    QList<Player*> getPlayerList();
    Team* getRedTeam();
    Team* getBlueTeam();
    QString getQueue(){return queue;}
    QStringList getNameList(){return nameList;}

private:
    QList<Card*> cardDB;
    QList<Player*> playerList;
    QList<Card*> handcards;
    Player* myself;
    Team *red,*blue,*otherTeam,*myTeam;
    QString queue;
    QStringList nameList;
    int id;
    int playerMax;
    int firstPlayerID;

};
extern DataInterface *dataInterface;
#endif // DATAINTERFACE_H
