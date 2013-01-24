#include "data/Player.h"
#include <QTextStream>
#include <QFile>
Player::Player(int ID, int isRed):id(ID),handCardsMax(6),crossNum(0),crossMax(2),energyMax(3),color(isRed),tap(0)
{
    gem=0;
    crystal=0;
    handCardsNum=0;
    for(int i=0;i<5;i++ )
        specials[i]=false;
}
void Player::setRole(int roleID)
{
    this->roleID=roleID;
    faceSource="role/"+QString::number(roleID)+".png";
    switch(roleID)
    {
    case 1:
        name=tr("[��ʥ]");
        break;
    case 2:
        name=tr("[��սʿ]");
        break;
    case 3:
        name=tr("[��֮Ů��]");
        break;
    case 4:
        name=tr("[��ӡʦ]");
        break;
    case 5:
        name=tr("[��ɱ��]");
        tapSource="resource/qianxing.png";
        break;
    case 6:
        name=tr("[ʥŮ]");
        tapSource="resource/lianmin.png";
        break;
    case 7:
        name=tr("[��ʹ]");
        break;
    case 8:
        name=tr("[ħ��ʦ]");
        break;
    case 9:
        name=tr("[ħ��]");
        tapSource="resource/anying.png";
        break;
    case 10:
        name=tr("[ʥǹ]");
        break;
    case 12:
        name=tr("[ð�ռ�]");
        break;
    }
}

void Player::addStatus(int type,Card* card)
{
    Status*status=new Status(type,card);
    statusList<<status;
    emit addStatusSIG(status);
}

void Player::removeStatus(Card* card)
{
    int i;
    for(i=0;i<statusList.count();i++)
        if(statusList.at(i)->card==card)
        {
            emit removeStatusSIG(statusList.at(i));
            statusList.removeAt(i);
        }
}

bool Player::checkStatus(int type)
{
    foreach(Status* ptr,statusList)
        if(ptr->type==type)
            return 1;
    return 0;
}

bool Player::checkStatus(QString status)
{
    foreach(Status* ptr,statusList)
        if(ptr->getCard()->getSpecialityList().contains(status))
            return 1;
    return 0;
}

void Player::setHandCardsMax(int howMany)
{
    handCardsMax=howMany;
}
void Player::setCrossNum(int howMany)
{
    crossNum=howMany;
}

void Player::setGem(int howMany)
{
    gem=howMany;
}

void Player::setCrystal(int howMany)
{
    crystal=howMany;
}

void Player::setEnergyMax(int howMany)
{
    energyMax=howMany;
}

void Player::setSpecial(int type,bool flag)
{
    specials[type]=flag;
}

int Player::getID()
{
    return id;
}

QString Player::getName()
{
    return name;
}

int Player::getHandCardMax()
{
    return handCardsMax;
}

int Player::getHandCardNum()
{
    return handCardsNum;
}

int Player::getCrossNum()
{
    return crossNum;
}
int Player::getCrossMax()
{
    return crossMax;
}

int Player::getGem()
{
    return gem;
}

int Player::getCrystal()
{
    return crystal;
}

int Player::getEnergy()
{
    return gem+crystal;
}

int Player::getColor()
{
    return color;
}

int Player::getEnergyMax()
{
    return energyMax;
}

