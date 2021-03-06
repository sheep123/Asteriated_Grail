#include "Characters.h"
#include "BackgroundEngine.h"
#include "Server.h"

Berserker::Berserker(BackgroundEngine *engine,int id,int color):PlayerEntity(engine,id,color)
{
    this->characterID = BERSERKER;
    this->star = 3;
    this->makeConnection(engine);
}

void Berserker::wild(QList<void*> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;

    Harm *harm = (Harm*)args[2];
    if(harm->type != ATTACK)
        return;
    harm->harmPoint++;
    coder.notice("狂战士发动【狂化】");
}

void Berserker::tear(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(this->getGem() <= 0)
        return;

    //此处询问是否使用
    coder.askForSkill(this->getID(),"撕裂");
    if(messageBuffer::readInfor() == 0)
        return;

    coder.notice("狂战士发动【撕裂】");
    this->setGem(this->getGem()-1);
    Harm *harm = (Harm*)args[2];
    harm->harmPoint += 2;
    coder.energyNotice(this->getID(),this->getGem(),this->getCrystal());
}

void Berserker::wildBladeAsk(QList<void*> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    this->wildBladeUsed = false;
    if(!*(bool*)args[4])
        return;
    CardEntity *card = (CardEntity*)args[3];
    if(!card->getHasSpeciality())
        return;
    if(card->getProperty() != tr("血"))
        return;
    if(!card->getSpecialityList().contains(tr("血影狂刀")))
        return;
    int handNum = ((PlayerEntity*)args[1])->getHandCardNum();
    if(handNum != 2 && handNum != 3)
        return;
    coder.askForSkill(this->getID(),"血影狂刀");
    if(messageBuffer::readInfor() == 1)
        this->wildBladeUsed = true;
    coder.notice("狂战士发动【血影狂刀】");
}

void Berserker::wildBlade(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(!*(bool*)args[4])
        return;
    CardEntity *card = (CardEntity*)args[3];
    if(!card->getHasSpeciality())
        return;
    if(card->getProperty() != tr("血"))
        return;
    if(card->getSpecialityList().contains(tr("血影狂刀")))
    {
        if(!this->wildBladeUsed)
            return;
        Harm *harm = (Harm*)args[2];
        switch(((PlayerEntity*)args[1])->getHandCards().length())
        {
        case 3:
            harm->harmPoint += 1;
            break;
        case 2:
            harm->harmPoint += 2;
        default:
            break;
        }
    }
}

void Berserker::roar(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(!*(bool*)args[4])
        return;
    CardEntity *card = (CardEntity*)args[3];
    if(!card->getHasSpeciality())
        return;
    if(card->getProperty() != tr("血"))
        return;

    if(card->getSpecialityList().contains(tr("血腥咆哮")))
    {
        if(((PlayerEntity*)args[1])->getCrossNum() == 2)
        {
            //询问是否发动
            coder.askForSkill(this->getID(),"血腥咆哮");
            if(messageBuffer::readInfor() == 0)
                return;
            coder.notice("狂战士发动【血腥咆哮】");
            *(int*)args[5] = NOMISS;
        }
    }
}

void Berserker::makeConnection(BackgroundEngine *engine)
{
    connect(engine,SIGNAL(timeLine3SIG(QList<void*>)),this,SLOT(wild(QList<void*>)));
    connect(engine,SIGNAL(timeLine2hitSIG(QList<void*>)),this,SLOT(tear(QList<void*>)));
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(wildBladeAsk(QList<void*>)));
    connect(engine,SIGNAL(timeLine2hitSIG(QList<void*>)),this,SLOT(wildBlade(QList<void*>)));
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(roar(QList<void*>)));
}

BowLady::BowLady(BackgroundEngine *engine, int id, int color):PlayerEntity(engine,id,color)
{
    this->characterID = BOWLADY;
    this->star = 3;
    this->makeConnection(engine);
}

void BowLady::thunderArrow(QList<void *> args)
{
    if(this != (PlayerEntity*)args[0])
        return;
    CardEntity* card = (CardEntity*)args[3];
    if(card->getElement() == tr("thunder"))
    {
        *((int*)args[5]) = NOREPLY;
        coder.notice("弓之女神发动【闪电箭】");
    }
}

void BowLady::transfixtion(QList<void *> args)
{
    if(this !=  (PlayerEntity*)args[0])
        return;
    if(!*(bool*)args[4] || handCards.size()==0)
        return;
    coder.askForSkill(this->getID(),"贯穿射击");
    BatInfor ans = messageBuffer::readBatInfor();
    if(ans.reply == 0)
        return;
    else
    {
        CardEntity *card = getCardByID(ans.CardID);
        QList<CardEntity*> cards;
        cards << card;
        this->removeHandCards(cards,true);
        coder.notice("弓之女神发动【贯穿射击】");
        coder.discardNotice(this->getID(),1,"y",cards);
        Harm transHurt;
        transHurt.harmPoint = 2;
        transHurt.type = MAGIC;
        engine->timeLine3(transHurt,this,(PlayerEntity*)args[1],"贯穿射击");
    }
}

void BowLady::accurateShoot(QList<void *> args)
{
    //精准射击连接于timeLine1SIG(QList<void*>)信号，同样我们应该先仔细查看信号的参数列表。
    //首先仍然是检查是否符合技能发动条件
    if(this !=  (PlayerEntity*)args[0])
        return;
    CardEntity* card = (CardEntity*)args[3];
    if(card->getProperty() != tr("技"))
        return;
    if(card->getHasSpeciality() == 0)
        return;
    if(card->getSpecialityList().contains(tr("精准射击")))
    {
        //此处使用35号通讯协议询问client是否发动技能
        coder.askForSkill(this->getID(),"精准射击");
        //请注意,messageBuffer::readInfor()会从缓冲中读取回复.这是个阻塞函数,直到它从缓冲中获得回复信息才会返回
        if(1 == messageBuffer::readInfor())
        {
            //根据36号协议,回复"1"表示玩家发动此技能
            //通过信号传递来的指针修改伤害数据
            Harm* harm = (Harm*)args[2];
            harm->harmPoint--;
            *(int*)args[5] = NOMISS;
            coder.notice("弓之女神发动【精准射击】");
        }
    }
}

void BowLady::trap(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != BOWLADY_TRAP)
        return;
    coder.notice("弓之女神发动【闪光陷阱】");
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    PlayerEntity* dst = engine->getPlayerByID(magic->dstID);
    this->engine->useCard(cards,this,dst);
    Harm harm;
    harm.harmPoint = 2;
    harm.type = MAGIC;
    this->engine->timeLine3(harm,this,dst,"闪光陷阱");
}

void BowLady::snipe(QList<void *> args)
{
    BatInfor* magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != BOWLADY_SNIPE)
        return;
    if(this->getGem() + this->getCrystal() < 1)
        return;
    coder.notice("弓之女神发动【狙击】");
    switch(magic->infor2)
    {
    case 0:
        this->crystal--;
        break;
    case 1:
        this->gem--;
        break;
    default:
        break;
    }
    coder.energyNotice(this->getID(),this->getGem(),this->getCrystal());

    PlayerEntity* dst = this->engine->getPlayerByID(magic->dstID);
    if(dst->getHandCardNum() < 5)
    {
        this->engine->drawCards(5 - dst->getHandCardNum(),NOTHARMED,dst);
    }
    this->engine->addActionNum(ATTACK);
}

void BowLady::makeConnection(BackgroundEngine *engine)
{
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(accurateShoot(QList<void*>)));
    connect(engine,SIGNAL(timeLine1ProSIG(QList<void*>)),this,SLOT(thunderArrow(QList<void*>)));
    connect(engine,SIGNAL(timeLine2missedSIG(QList<void*>)),this,SLOT(transfixtion(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(trap(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(snipe(QList<void*>)));
}

MoDao::MoDao(BackgroundEngine* engine,int id,int color):PlayerEntity(engine,id,color)
{
    this->characterID = 8;
    this->star = 3;
    this->makeConnection(engine);
}

void MoDao::makeConnection(BackgroundEngine* engine)
{
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(MoDanRongHe(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(MoBaoChongJi(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(HuiMeiFengBao(QList<void*>)));
}

void MoDao::MoDanRongHe(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 801 && magic->infor1 != 802)
        return;
    coder.notice("魔导师发动【魔弹融合】");
    QList<CardEntity*> cards;
    CardEntity* card;
    card=getCardByID(magic->CardID);
    cards << card;
    engine->useCard(cards,this,engine->getPlayerByID(magic->dstID));
    this->engine->missileProcess(card,magic->srcID,magic->dstID);
}

void MoDao::MoBaoChongJi(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 803)
        return;
    int dst1=magic->dstID;
    int dst2=magic->infor2;
    QString msg;
    msg="魔导师对玩家"+QString::number(dst1)+"和玩家"+QString::number(dst2)+"发动【魔爆冲击】";
    coder.notice(msg);
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    coder.discardNotice(this->getID(),1,"y",cards);
    this->removeHandCards(cards,true);

    bool flag1,flag2,flag;
    flag1=flag2=true;
    flag=false;
    PlayerEntity* ptr = engine->getNext(this);
    do
    {
        if(flag1 && ptr->getID()==dst1)
        {
            flag1=false;
            coder.askForDiscardMagic(dst1);

            BatInfor ans = messageBuffer::readBatInfor();
            if(ans.reply == 0)
            {
                Harm MoBao;
                MoBao.harmPoint = 2;
                MoBao.type = MAGIC;
                engine->timeLine3(MoBao,this,ptr,"魔爆冲击");
                flag=true;
                if(engine->checkEnd())
                    break;
            }
            else
            {
                CardEntity *card = getCardByID(ans.CardID);
                QList<CardEntity*> cards;
                cards << card;
                ptr->removeHandCards(cards,true);
                coder.discardNotice(dst1,1,"y",cards);
            }
        }
        else if(flag2 && ptr->getID()==dst2)
        {
            flag2=false;
            coder.askForDiscardMagic(dst2);

            BatInfor ans = messageBuffer::readBatInfor();
            if(ans.reply == 0)
            {
                Harm MoBao;
                MoBao.harmPoint = 2;
                MoBao.type = MAGIC;
                engine->timeLine3(MoBao,this,ptr,"魔爆冲击");
                flag=true;
                if(engine->checkEnd())
                    break;
            }
            else
            {
                CardEntity *card = getCardByID(ans.CardID);
                QList<CardEntity*> cards;
                cards << card;
                ptr->removeHandCards(cards,true);
                coder.discardNotice(dst2,1,"y",cards);
            }
        }
        ptr = engine->getNext(ptr);
    }while(flag1||flag2);
    if(flag)
    {
        teamArea.setGem(color,teamArea.getGem(color)+1);
        coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));
    }

}

void MoDao::HuiMeiFengBao(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 804)
        return;
    int dst1=magic->dstID;
    int dst2=magic->infor2;
    QString msg;
    msg="魔导师对玩家"+QString::number(dst1)+"和玩家"+QString::number(dst2)+"发动【毁灭风暴】";
    coder.notice(msg);
    this->gem--;
    coder.energyNotice(this->getID(),this->getGem(),this->getCrystal());

    bool flag1,flag2;
    flag1=flag2=true;
    PlayerEntity* ptr = engine->getNext(this);
    do
    {
        if(flag1 && ptr->getID()==dst1)
        {
            flag1=false;
            Harm HuiMie;
            HuiMie.harmPoint = 2;
            HuiMie.type = MAGIC;
            engine->timeLine3(HuiMie,this,ptr,"毁灭风暴");
            if(engine->checkEnd())
                break;
        }
        else if(flag2 && ptr->getID()==dst2)
        {
            flag2=false;
            Harm HuiMie;
            HuiMie.harmPoint = 2;
            HuiMie.type = MAGIC;
            engine->timeLine3(HuiMie,this,ptr,"毁灭风暴");
            if(engine->checkEnd())
                break;
        }
        ptr = engine->getNext(ptr);
    }while(flag1||flag2);
}
AnSha:: AnSha(BackgroundEngine* engine,int id,int color):PlayerEntity(engine,id,color)
{
    this->characterID = 5;
    this->star = 3;
    this->makeConnection(engine);
}

void AnSha::makeConnection(BackgroundEngine* engine)
{
    connect(engine,SIGNAL(timeLine6DrawedSIG(QList<void*>)),this,SLOT(FanShi(QList<void*>)));
    connect(engine,SIGNAL(timeLine3SIG(QList<void*>)),this,SLOT(ShuiYing(QList<void*>)));
    connect(engine,SIGNAL(actionPhaseSIG(QList<void*>)),this,SLOT(QianXing(QList<void*>)));
    connect(engine,SIGNAL(timeLine1ProSIG(QList<void*>)),this,SLOT(AttackCheck(QList<void*>)));
}

void AnSha::ShuiYing(QList<void*> args)
{
    int howMany;
    if(this != ((PlayerEntity*)args[1])||handCards.size()==0)
        return;
    coder.askForSkill(this->getID(),"水影");

    QString msg=messageBuffer::readMsg();
    QStringList arg=msg.split(";");
    howMany=arg[0].toInt();
    if(howMany == 0)
        return;
    else
    {
        QList<CardEntity*>cards;
        arg=arg[1].split(",");
        int i;
        for(i=0;i<howMany;i++)
        {
            cards<<getCardByID(arg[i].toInt());
        }
        this->removeHandCards(cards,true);
        coder.notice("暗杀者发动【水影】");
        coder.discardNotice(this->getID(),howMany,"y",cards);
    }
}

void AnSha::QianXing(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0])
        return;
    if(tap==1)
    {
        setTap(0);
        coder.tapNotice(id,0,"【普通形态】");
        handCardsMax++;
        coder.handcardMaxNotice(id,handCardsMax);
    }
    if(getGem()==0)
        return;
    coder.askForSkill(id,"潜行");
    int reply=messageBuffer::readInfor();
    if(reply==0)
        return;
    this->gem--;
    coder.energyNotice(id,this->getGem(),this->getCrystal());
    setTap(1);
    coder.tapNotice(id,1,"【潜行形态】");
    handCardsMax--;
    coder.handcardMaxNotice(id,handCardsMax);
    setHandCardsMax(handCardsMax);
}
/*
#define NOTHARMED  0
#define ATTACKHARM 1
#define MAGICHARM  2
*/
void AnSha::FanShi(QList<void*> args)
{
    if(this != (PlayerEntity*)args[1] || ((Harm*)args[2])->type!=1)
            return;
    coder.notice("暗杀者发动【反噬】");
    engine->drawCards(1,0,(PlayerEntity*)args[0]);

}

void AnSha::AttackCheck(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0] || tap==0)
        return;
    if(*(bool*)args[4])
    {
        ((Harm*)args[2])->harmPoint = 2+getEnergy();
        *((int*)args[5]) = NOREPLY;
        coder.notice("暗杀者发动【潜行】效果");
    }
}

TianShi::TianShi(BackgroundEngine* engine,int id,int color):PlayerEntity(engine,id,color)
{
    this->characterID = 7;
    this->star = 3;
    this->makeConnection(engine);
}

void TianShi::makeConnection(BackgroundEngine* engine)
{
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(FengZhiJieJin(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(TianShiZhuFu(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(TianShiZhiQiang(QList<void*>)));
    connect(engine,SIGNAL(turnBeginPhaseSIG(QList<void*>)),this,SLOT(TianShiZhiGe(QList<void*>)));
    connect(engine,SIGNAL(loseMoraleSIG(int,int*,PlayerEntity*)),this,SLOT(ShenZhiBiHu(int,int*,PlayerEntity*)));
    connect(engine,SIGNAL(usedShield(int)),this,SLOT(TianShiJiBan(int)));
}

void TianShi::FengZhiJieJin(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id || magic->infor1 != 701)
        return;
    coder.notice("天使发动【风之洁净】");
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    coder.discardNotice(this->getID(),1,"y",cards);
    this->removeHandCards(cards,true);
    int dstID=magic->dstID;
    PlayerEntity* ptr=engine->getPlayerByID(dstID);
    CardEntity*card=getCardByID(magic->infor2);
    if(ptr->removeBasicEffect(card))
    {
        coder.notice("天使移除玩家"+QString::number(dstID)+"面前的"+card->getName());
        TianShiJiBan(id);
    }
}

void TianShi::TianShiZhuFu(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id || magic->infor1 != 702)
        return;
    bool flag1,flag2;
    int n=2;
    flag1=true;
    flag2=false;
    int howMany=magic->infor2;
    int dst1=magic->dstID;
    int dst2=-1;

    QString msg;
    msg="天使对玩家"+QString::number(dst1);
    if(howMany==2)
    {
        dst2=magic->infor3;
        msg+="和玩家"+QString::number(dst2);
        flag2=true;
        n=1;
    }
    msg+="发动【天使祝福】";
    coder.notice(msg);
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    coder.discardNotice(this->getID(),1,"y",cards);
    this->removeHandCards(cards,true);

    PlayerEntity* ptr = engine->getNext(this);
    do
    {
        if(flag1 && ptr->getID()==dst1)
        {
            cards.clear();
            flag1=false;
            if(engine->getPlayerByID(dst1)->getHandCardNum()>=n)
            {
                coder.askToGiveCard(dst1,n);
                cards=messageBuffer::readCardID(n);
                ptr->giveHandCards(cards,this);
            }
            else if(engine->getPlayerByID(dst1)->getHandCardNum()==1)
            {
                coder.askToGiveCard(dst1,1);
                cards=messageBuffer::readCardID(1);
                ptr->giveHandCards(cards,this);
            }
        }
        else if(flag2 && ptr->getID()==dst2)
        {
            cards.clear();
            flag2=false;
            if(engine->getPlayerByID(dst2)->getHandCardNum()>=n)
            {
                coder.askToGiveCard(dst2,n);
                cards=messageBuffer::readCardID(n);
                ptr->giveHandCards(cards,this);
            }
        }
        ptr = engine->getNext(ptr);
    }while(flag1||flag2);
}

void TianShi::TianShiZhiQiang(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id || magic->infor1 != 703)
        return;
    coder.notice("天使发动【天使之墙】");
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    PlayerEntity* dst = engine->getPlayerByID(magic->dstID);
    engine->useCard(cards,this,dst,true);
}

void TianShi::TianShiZhiGe(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0] || gem+crystal==0)
        return;
    PlayerEntity*ptr=this;
    int i,n=engine->getPlayerNum();
    for(i=0;i<n;i++)
    {
        if(!ptr->getBasicEffect().empty())
            break;
        ptr=ptr->getNext();
    }
    if(i==n)
        return;
    coder.askForSkill(id,"天使之歌");
    BatInfor ans = messageBuffer::readBatInfor();
    if(ans.reply==0)
        return;
    if(ans.infor1)
        gem--;
    else
        crystal--;
    coder.energyNotice(id,gem,crystal);
    int dstID=ans.dstID;
    ptr=engine->getPlayerByID(dstID);
    CardEntity*card=getCardByID(ans.CardID);
    if(ptr->removeBasicEffect(card))
    {
        coder.notice("天使移除玩家"+QString::number(dstID)+"面前的"+card->getName());
        TianShiJiBan(id);
    }
}

/*
#define NOTHARMED  0
#define ATTACKHARM 1
#define MAGICHARM  2
*/
void TianShi::ShenZhiBiHu(int harmed,int*howMany,PlayerEntity*dst)
{
    if(harmed!=2 || dst->getColor()!=color || gem+crystal==0|| *howMany==0)
        return;
    coder.askForSkill(id,"神之庇护",TOQSTR(*howMany));
    BatInfor ans = messageBuffer::readBatInfor();
    if(ans.reply == 0)
        return;
    else
    {
        *howMany-=ans.infor1+ans.infor2;
        gem-=ans.infor1;
        crystal-=ans.infor2;
        coder.energyNotice(this->getID(),gem,crystal);
        coder.notice("天使发动【神之庇护】抵御"+TOQSTR(ans.infor1+ans.infor2)+"点士气下降");
    }

}

void TianShi::TianShiJiBan(int userID)
{
    if(userID!=id)
        return;
    coder.askForSkill(id,"天使羁绊");
    BatInfor ans = messageBuffer::readBatInfor();
    int dstID=ans.dstID;
    PlayerEntity* player= engine->getPlayerByID(dstID);
    coder.notice("天使对玩家"+TOQSTR(dstID)+"发动【天使羁绊】");
    int cross=player->getCrossNum();
    if(player->getCrossMax()>cross)
    {
        player->setCrossNum(cross+1);
        coder.crossChangeNotice(dstID,cross+1);
    }
}
//圣女初始化
Saintness::Saintness(BackgroundEngine *engine, int id, int color):PlayerEntity(engine,id,color)
{
    this->characterID = 6;
    this->star = 3;
    this->makeConnection(engine);
}

//冰霜祷言
void Saintness::prayerOfFrost(QList<void *> args)
{
    if(this != (PlayerEntity*)args[0])
        return;
    CardEntity* card = (CardEntity*)args[3];
    if(card->getElement() != tr("water"))
        return;
    coder.askForSkill(this->getID(),"冰霜祷言");
    BatInfor ans = messageBuffer::readBatInfor();
    int dstID= ans.dstID;
    PlayerEntity* player= engine->getPlayerByID(dstID);
    coder.notice("圣女对玩家"+TOQSTR(dstID)+"发动【冰霜祷言】");
    int cross=player->getCrossNum();
    if(player->getCrossMax()>cross)
    {
        player->setCrossNum(cross+1);
        coder.crossChangeNotice(dstID,cross+1);
    }
}

//治疗术
void Saintness::cure(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 602)
        return;

    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    PlayerEntity* dst = engine->getPlayerByID(magic->dstID);
    int dstID=dst->getID();
    coder.notice("圣女对玩家"+TOQSTR(dstID)+"发动【治疗术】");
    this->engine->useCard(cards,this,dst);        
    int cross = dst->getCrossNum();
    int max = dst->getCrossMax();
    if (cross<max)
    {
        cross+=2;
        if (cross > max)
            cross = max;
        dst->setCrossNum(cross);
        coder.crossChangeNotice(dstID, cross);
    }
}

//治愈之光
void Saintness::healingLight(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 603)
        return;
    coder.notice("圣女发动【治愈之光】");
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    PlayerEntity* player = engine->getPlayerByID(magic->dstID);
    this->engine->useCard(cards,this,player);
    int dst[3], n, cross;
    n=magic->infor2;
    dst[0] = magic->dstID;
    if (n>1)
        dst[1] = magic->infor3;
    if (n>2)
        dst[2] = magic->infor4;
    for(int i=0;i<n; i++)
    {
        player = engine->getPlayerByID(dst[i]);
        coder.notice("圣女使用【治愈之光】为玩家"+TOQSTR(dst[i])+"增加1点治疗");
        cross=player->getCrossNum();
        if(player->getCrossMax()>cross)
        {
            player->setCrossNum(cross+1);
            coder.crossChangeNotice(dst[i],cross+1);
        }
    }
}

//怜悯
void Saintness::mercy(QList<void *> args)
{
    if(this != (PlayerEntity*)args[0])
        return;
    if(tap == 1)
        return;
    if(getGem()==0)
        return;
    coder.askForSkill(this->getID(),"怜悯");
    int reply=messageBuffer::readInfor();
    if(reply==0)
        return;
    this->gem--;
    coder.energyNotice(this->getID(),this->getGem(),this->getCrystal());
    setTap(1);
    handCardsMax++;
    setHandCardsMax(handCardsMax);
    setHandCardsMaxFixed(true);
    coder.tapNotice(id,1,"【怜悯形态】");
    coder.handcardMaxNotice(id,handCardsMax);
}

//圣疗
void Saintness::layOnHands(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 605)
        return;
    int dst[3], addcross[3];
    dst[0] = magic->dstID;
    dst[1] = magic->infor2;
    dst[2] = magic->infor3;
    PlayerEntity* player;
    for(int i=0; i<3; i++)
        addcross[i]=0;
    switch(magic->infor4)
    {
    case 0:
        this->crystal--;
        break;
    case 1:
        this->gem--;
        break;
    default:
        break;
    }
    coder.energyNotice(this->getID(),this->getGem(),this->getCrystal());
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            if(dst[i] == dst[j])
            {
                addcross[j]++;
                break;
            }
    for (int i=0; i<3; i++)
    {
        if(addcross[i] == 0)
            continue;
        player = engine->getPlayerByID(dst[i]);
        coder.notice("圣女使用【圣疗】为玩家"+TOQSTR(dst[i])+"增加"+TOQSTR(addcross[i])+"治疗");
        int cross = player->getCrossNum();
        int max = player->getCrossMax();
        if (cross<max)
        {
            cross+=addcross[i];
            if (cross > max)
                cross = max;
            player->setCrossNum(cross);
            coder.crossChangeNotice(dst[i], cross);
        }
    }
    this->engine->addActionNum(ATTACKORMAGIC);
}

//
void Saintness::makeConnection(BackgroundEngine *engine)
{
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(prayerOfFrost(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(cure(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(healingLight(QList<void*>)));
    connect(engine,SIGNAL(actionPhaseSIG(QList<void*>)),this,SLOT(mercy(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(layOnHands(QList<void*>)));
}

JianSheng::JianSheng(BackgroundEngine *engine,int id,int color):PlayerEntity(engine,id,color)
{
    this->characterID = 1;
    this->star = 3;
    this->makeConnection(engine);
}

//烈风技
void JianSheng::LieFengJi1(QList<void*> args)
{
    LieFengJiUsed=false;
    if(this != (PlayerEntity*)args[0])
        return;
    CardEntity* card = (CardEntity*)args[3];
    if(!card->getSpecialityList().contains(tr("烈风技")))
        return;
    PlayerEntity* dst=(PlayerEntity*)args[1];
    bool flag=false;
    for(int i = 0;i < dst->getBasicEffect().size();i++)
        if(dst->getBasicEffect().at(i)->getMagicName() == SHIELDCARD || dst->getBasicEffect().at(i)->getSpecialityList().contains(tr("天使之墙")))
        {
            flag=true;
            break;
        }
    if(!flag)
        return;
    coder.askForSkill(id,"烈风技");
    if(messageBuffer::readInfor() == 0)
        return;
    *((int*)args[5]) = NOREPLY;
    coder.notice("剑圣发动【烈风技】");
    LieFengJiUsed=true;
}

void JianSheng::LieFengJi2(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0])
        return;
    if(!LieFengJiUsed)
        return;
    *((bool*)args[2]) =  false;
}

//疾风技
void JianSheng::JiFengJi(QList<void *> args)
{
    PlayerEntity* myself=(PlayerEntity*)args[0];
    if(this != myself ||!*(bool*)args[4])
        return;
    CardEntity* card = (CardEntity*)args[3];
    if(!card->getSpecialityList().contains(tr("疾风技")))
        return;
    coder.notice("剑圣发动【疾风技】");
    engine->addActionNum(ATTACK);
}
//连续技
void JianSheng::LianXuJi1(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(getHandCardNum()<=0 ||LianXuJiUsed)
        return;
    engine->addActionNum(ATTACK);
    LianXuJiUsed=true;
}
void JianSheng::LianXuJi2(QList<void *> args)
{
    BatInfor *skill = (BatInfor*)args[0];
    if(id != skill->srcID||skill->infor1!=101)
        return;
    coder.notice("剑圣发动【连续技】");
}

//剑影
void JianSheng::JianYing1(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(this->getEnergy() <= 0 || getHandCardNum()<=0 ||JianYingUsed)
        return;
    engine->addActionNum(ATTACK);
    JianYingUsed=true;
}
void JianSheng::JianYing2(QList<void *> args)
{
    BatInfor *skill = (BatInfor*)args[0];
    if(id != skill->srcID||skill->infor1!=102)
        return;
    if(skill->infor2==0)
        crystal--;
    else
        gem--;
    coder.notice("剑圣发动【剑影】");
    coder.energyNotice(id,gem,crystal);
}

void JianSheng::skillReset(QList<void*>args){
    if(this != ((PlayerEntity*)args[0]))
        return;
    JianYingUsed=false;
    LianXuJiUsed=false;
    attackCount=0;
}
//圣剑
void JianSheng::ShengJian(QList<void *> args){
    if(this != ((PlayerEntity*)args[0])||!*(bool*)args[4])
        return;
    attackCount++;
    if(attackCount==3){
        coder.notice("剑圣发动【圣剑】");
        *((int*)args[5]) = NOMISS;
    }
}

void JianSheng::makeConnection(BackgroundEngine *engine)
{    
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(LieFengJi1(QList<void*>)));
    connect(engine,SIGNAL(shieldSIG(QList<void*>)),this,SLOT(LieFengJi2(QList<void*>)));
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(JiFengJi(QList<void*>)));
    connect(engine,SIGNAL(attackFinishSIG(QList<void*>)),this,SLOT(LianXuJi1(QList<void*>)));
    connect(engine,SIGNAL(skillAttack(QList<void*>)),this,SLOT(LianXuJi2(QList<void*>)));
    connect(engine,SIGNAL(turnBeginPhaseSIG(QList<void*>)),this,SLOT(skillReset(QList<void*>)));
    connect(engine,SIGNAL(attackFinishSIG(QList<void*>)),this,SLOT(JianYing1(QList<void*>)));
    connect(engine,SIGNAL(skillAttack(QList<void*>)),this,SLOT(JianYing2(QList<void*>)));
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(ShengJian(QList<void*>)));
}

FengYin::FengYin(BackgroundEngine *engine, int id, int color):PlayerEntity(engine,id,color)
{
    this->characterID = 4;
    this->star = 3;
    this->makeConnection(engine);
    shuFuID=-1;
}
//法术激荡
void FengYin::FaShuJiDang1(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(getHandCardNum()<=0)
        return;
    engine->addActionNum(ATTACK);
}
void FengYin::FaShuJiDang2(QList<void *> args)
{
    BatInfor *skill = (BatInfor*)args[0];
    if(id != skill->srcID||skill->infor1!=404)
        return;
    coder.notice("封印师发动【法术激荡】");
}

//封印法术
void FengYin::FengYinFaShu1(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id || magic->infor1 != 401)
        return;
    coder.notice("封印师发动【封印法术】");
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    PlayerEntity* dst = engine->getPlayerByID(magic->dstID);
    engine->useCard(cards,this,dst,true);
}
void FengYin::FengYinFaShu2(QList<CardEntity*> cards,PlayerEntity* user)
{
    foreach(CardEntity*fengYin, user->getBasicEffect())
        if(fengYin->getType()=="attack" && fengYin->getProperty()==tr("幻"))
            foreach(CardEntity*ptr, cards)
                if(ptr->getElement()==fengYin->getElement())
                {
                    coder.notice("玩家"+QString::number(user->getID())+"触发"+fengYin->getSpecialityList().at(0));
                    user->removeBasicEffect(fengYin);
                    Harm harm;
                    harm.harmPoint = 3;
                    harm.type = MAGIC;
                    engine->timeLine3(harm,this,user,fengYin->getSpecialityList().at(0));
                    break;
                }
}

//五系束缚
void FengYin::WuXiShuFu1(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id || magic->infor1 != 402)
        return;
    if(magic->infor2)
        gem--;
    else
        crystal--;
    shuFuID=magic->dstID;
    coder.notice("封印师对玩家"+QString::number(shuFuID)+"发动【五系束缚】");
    coder.energyNotice(id,gem,crystal);
    coder.specialNotice(shuFuID,0,1);
}

void FengYin::WuXiShuFu2(PlayerEntity* player, bool* withoutTrap)
{
    if(player->getID()!=shuFuID)
        return;
    bool weak=false;
    *withoutTrap=true;
    int howMany=2;
    PlayerEntity* ptr=player;
    for(int i=0;i<engine->getPlayerNum();i++)
    { 
        foreach(CardEntity*card,ptr->getBasicEffect())
            if(card->getType()=="attack"&& card->getProperty()==tr("幻"))
                howMany++;
        ptr=ptr->getNext();
    }
    if(howMany>4)
        howMany=4;
    for(int i = 0;i < player->getBasicEffect().size();i++)
        if(player->getBasicEffect().at(i)->getMagicName() == WEAKCARD)
        {
            engine->weakProcess(player,3+howMany);
            player->removeBasicEffect(player->getBasicEffect()[i]);
            weak=true;
            break;
        }
    if(!weak)
        engine->weakProcess(player,howMany);
    coder.specialNotice(shuFuID,0,0);
    shuFuID=-1;
}

//封印破碎
void FengYin::FengYinPoSui(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id || magic->infor1 != 403)
        return;
    coder.notice("封印师发动【封印破碎】");
    if(magic->infor2)
        gem--;
    else
        crystal--;
    coder.energyNotice(id,gem,crystal);
    int dstID=magic->dstID;
    PlayerEntity* ptr=engine->getPlayerByID(dstID);
    CardEntity*card=getCardByID(magic->CardID);
    QList<CardEntity*>cards;
    cards.append(card);
    if(ptr->removeBasicEffect(card,id,HAND))
    {
        coder.notice("封印师收走玩家"+QString::number(dstID)+"面前的"+card->getName());
        coder.getCardNotice(1,cards,id,true);
        addHandCards(cards,0,false);
    }
}

void FengYin::makeConnection(BackgroundEngine *engine)
{
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(FengYinFaShu1(QList<void*>)));
    connect(engine,SIGNAL(showHandCards(QList<CardEntity*>,PlayerEntity*)),this,SLOT(FengYinFaShu2(QList<CardEntity*>,PlayerEntity*)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(WuXiShuFu1(QList<void*>)));
    connect(engine,SIGNAL(weakSIG(PlayerEntity*,bool*)),this,SLOT(WuXiShuFu2(PlayerEntity*,bool*)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(FengYinPoSui(QList<void*>)));
    connect(engine,SIGNAL(magicFinishSIG(QList<void*>)),this,SLOT(FaShuJiDang1(QList<void*>)));
    connect(engine,SIGNAL(skillAttack(QList<void*>)),this,SLOT(FaShuJiDang2(QList<void*>)));
}

MoJian::MoJian(BackgroundEngine *engine,int id,int color):PlayerEntity(engine,id,color)
{
    this->characterID = 9;
    this->star = 3.5;
    this->makeConnection(engine);
}

//黑暗震颤
void MoJian::HeiAnZhenChan1(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0]||HeiAnZhenChanUsed||gem<=0||!*(bool*)args[4])
        return;
    coder.askForSkill(id,"黑暗震颤");
    if(messageBuffer::readInfor() == 0)
        return;
    *((int*)args[5]) = NOREPLY;
    gem--;
    coder.energyNotice(id,gem,crystal);
    coder.notice("魔剑发动【黑暗震颤】");
    HeiAnZhenChanUsed=true;
}

void MoJian::HeiAnZhenChan2(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0]||!HeiAnZhenChanUsed)
        return;
    engine->drawCards(getHandCardMax()-getHandCardNum(),0,this);
}

//修罗连斩
void MoJian::XiuLuoLianZhan1(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(getHandCardNum()<=0 ||XiuLuoLianZhanUsed)
        return;
    engine->addActionNum(ATTACK);
    XiuLuoLianZhanUsed=true;
}

void MoJian::XiuLuoLianZhan2(QList<void *> args)
{
    BatInfor *skill = (BatInfor*)args[0];
    if(id != skill->srcID||skill->infor1!=901)
        return;
    coder.notice("魔剑发动【修罗连斩】");
}

void MoJian::skillReset(QList<void*>args){
    if(this != ((PlayerEntity*)args[0]))
        return;
    XiuLuoLianZhanUsed=false;
    HeiAnZhenChanUsed=false;
}

//暗影凝聚
void MoJian::AnYingNingJu(QList<void*> args)
{
    if(this != (PlayerEntity*)args[0])
        return;
    if(tap==1)
    {
        setTap(0);
        coder.tapNotice(id,0,"【普通形态】");
    }
    coder.askForSkill(this->getID(),"暗影凝聚");
    int reply=messageBuffer::readInfor();
    if(reply==0)
        return;
    coder.notice("魔剑发动【暗影凝聚】");
    Harm anying;
    anying.harmPoint=1;
    anying.type=MAGICHARM;
    engine->timeLine3(anying,this,this,"暗影凝聚");
    setTap(1);
    coder.tapNotice(id,1,"【暗影形态】");
}

//暗影之力
void MoJian::AnYingZhiLi(QList<void*> args)
{
    if(this != ((PlayerEntity*)args[0])||!tap)
        return;

    Harm *harm = (Harm*)args[2];
    if(harm->type != ATTACK)
        return;
    harm->harmPoint++;
    coder.notice("魔剑发动【暗影之力】");
}

//暗影流星
void MoJian::AnYingLiuXing(QList<void*> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 902)
        return;
    coder.notice("魔剑发动【暗影流星】");
    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    cards << getCardByID(magic->infor2);
    PlayerEntity* dst = engine->getPlayerByID(magic->dstID);
    coder.discardNotice(id,2,"y",cards);
    this->removeHandCards(cards,true);

    Harm harm;
    harm.harmPoint = 2;
    harm.type = MAGIC;
    this->engine->timeLine3(harm,this,dst,"暗影流星");
}

void MoJian::makeConnection(BackgroundEngine *engine)
{
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(AnYingLiuXing(QList<void*>)));
    connect(engine,SIGNAL(attackFinishSIG(QList<void*>)),this,SLOT(XiuLuoLianZhan1(QList<void*>)));
    connect(engine,SIGNAL(skillAttack(QList<void*>)),this,SLOT(XiuLuoLianZhan2(QList<void*>)));
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(HeiAnZhenChan1(QList<void*>)));
    connect(engine,SIGNAL(timeLine2hitSIG(QList<void*>)),this,SLOT(HeiAnZhenChan2(QList<void*>)));
    connect(engine,SIGNAL(turnBeginPhaseSIG(QList<void*>)),this,SLOT(skillReset(QList<void*>)));
    connect(engine,SIGNAL(actionPhaseSIG(QList<void*>)),this,SLOT(AnYingNingJu(QList<void*>)));
    connect(engine,SIGNAL(timeLine3SIG(QList<void*>)),this,SLOT(AnYingZhiLi(QList<void*>)));

}

MaoXian::MaoXian(BackgroundEngine *engine, int id, int color):PlayerEntity(engine,id,color)
{
    this->characterID = 12;
    this->star = 3.5;
    this->makeConnection(engine);
}
void MaoXian::makeConnection(BackgroundEngine *engine)
{
    connect(engine,SIGNAL(skillAttack(QList<void*>)),this,SLOT(QiZha(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(TouTianHuanRi(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(TeShuJiaGong(QList<void*>)));
    connect(engine,SIGNAL(skillSpecial(QList<void*>)),this,SLOT(MaoXianZheTianTang(QList<void*>)));
}

void MaoXian::QiZha(QList<void *> args)
{
    BatInfor *skill = (BatInfor*)args[0];
    if(id != skill->srcID||skill->infor1!=1201)
        return;
    coder.notice("冒险家发动【欺诈】");
    setCrystal(crystal+1);
    coder.energyNotice(id,gem,crystal);
    QList<CardEntity*>cards;
    cards.append(getCardByID(skill->infor3));
    cards.append(getCardByID(skill->infor4));
    if(skill->infor2==3)
        cards.append(getCardByID(skill->infor5));
    *(int*)args[1]=0;
    coder.discardNotice(id,skill->infor2,"y",cards);
    this->removeHandCards(cards,true);
}

//偷天换日
void MaoXian::TouTianHuanRi(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id)
        return;
    if(magic->infor1 != 1203)
        return;
    if(magic->infor2)
        gem--;
    else
        crystal--;
    coder.notice("冒险家发动【偷天换日】");
    coder.energyNotice(id,gem,crystal);
    teamArea.setGem(color,teamArea.getGem(color)+1);
    coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));
    int other=1-color;
    teamArea.setGem(other,teamArea.getGem(other)-1);
    coder.stoneNotice(other,teamArea.getGem(other),teamArea.getCrystal(other));
    engine->addActionNum(ATTACKORMAGIC);
}

//特殊加工
void MaoXian::TeShuJiaGong(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id)
        return;
    if(magic->infor1 != 1202)
        return;
    if(magic->infor2)
        gem--;
    else
        crystal--;
    coder.notice("冒险家发动【特殊加工】");
    coder.energyNotice(id,gem,crystal);
    int toChange=teamArea.getCrystal(color);
    teamArea.setCrystal(color,0);
    teamArea.setGem(color,toChange+teamArea.getGem(color));
    coder.stoneNotice(color,teamArea.getGem(color),0);
    engine->addActionNum(ATTACKORMAGIC);
}
//冒险者天堂
void MaoXian::MaoXianZheTianTang(QList<void *> args)
{
    BatInfor *magic = (BatInfor*)args[0];
    if(magic->srcID != id)
        return;
    if(magic->infor1 != 1204)
        return;
    coder.notice("冒险家对玩家"+QString::number(magic->dstID)+"发动【冒险者天堂】");
    PlayerEntity*dst=engine->getPlayerByID(magic->dstID);
    dst->setGem(dst->getGem()+magic->infor2);
    dst->setCrystal(dst->getCrystal()+magic->infor3);
    teamArea.setGem(color,teamArea.getGem(color)-magic->infor2);
    teamArea.setCrystal(color,teamArea.getCrystal(color)-magic->infor3);
    coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));
    coder.energyNotice(magic->dstID,dst->getGem(),dst->getCrystal());
}

/******************
  圣枪 10
  *******************/
ShengQiang::ShengQiang(BackgroundEngine *engine, int id, int color):PlayerEntity(engine, id, color)
{
    this->characterID = 10;
    this->star = 3.5;
    //神圣信仰
    this->crossMax = 3;
    this->makeConnection(engine);
}

//辉耀
void ShengQiang::HuiYao(QList<void *> args)
{
     BatInfor *magic = (BatInfor*)args[0];
     if(magic->srcID != this->getID())
         return;
     if(magic->infor1 != 1001)
         return;

     QList<CardEntity*> cards;
     cards << getCardByID(magic->CardID);
     coder.notice("圣枪发动【辉耀】");
     this->removeHandCards(cards,true);
     coder.discardNotice(this->getID(),1,"y",cards);
     PlayerEntity* dst = this;
     int cross, max;
     do{
         cross = dst->getCrossNum();
         max = dst->getCrossMax();
         if (cross<max)
         {
             cross++;
             dst->setCrossNum(cross);
             coder.crossChangeNotice(dst->getID(), cross);
         }
         dst = dst->getNext();
     }while(dst!= this);
     this->engine->addActionNum(ATTACK);
}
//惩戒
void ShengQiang::ChengJie(QList<void *> args)
{
    BatInfor* magic =(BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 1002)
        return;

    QList<CardEntity*> cards;
    cards << getCardByID(magic->CardID);
    coder.notice("圣枪对玩家"+QString::number(magic->dstID)+"发动【惩戒】");
    this->removeHandCards(cards,true);
    coder.discardNotice(this->getID(), 1, "y", cards);
    PlayerEntity* dst = engine->getPlayerByID(magic->dstID);
    int cross, max;
    cross = dst->getCrossNum();
    cross--;
    dst->setCrossNum(cross);
    coder.crossChangeNotice(dst->getID(), cross);
    coder.notice("玩家"+QString::number(magic->dstID)+"减少1点治疗，圣枪增加1治疗");
    cross = this->getCrossNum();
    max = this->getCrossMax();
    if (cross<max)
    {
        cross ++;
        this->setCrossNum(cross);
        coder.crossChangeNotice(this->getID(), cross);
    }
    this->engine->addActionNum(ATTACK);
}

//圣击，只判断非主动攻击
void ShengQiang::ShengJi(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(*(bool*)args[4])
        return;
    int cross = this->getCrossNum();
    int max = this->getCrossMax();
    if(cross < max)
    {
        cross ++;
        this->setCrossNum(cross);
        coder.crossChangeNotice(this->getID(), cross);
    }
    coder.notice("圣枪发动【圣击】，增加1治疗");
}

//天枪
void ShengQiang::TianQiang(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(!*(bool*)args[4])
        return;
    if(this->ShengGuangQiYuUsed)
        return;
    int cross = this->getCrossNum();
    if(cross<2)
        return;
    coder.askForSkill(this->getID(),"天枪");
    if(messageBuffer::readInfor() == 0)
        return;
    cross-=2;
    this->setCrossNum(cross);
    coder.crossChangeNotice(this->getID(), cross);
    coder.notice("圣枪发动【天枪】");
    *(int*)args[5] = NOREPLY;
}

//地枪，不发动则发动圣击
void ShengQiang::DiQiang(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    if(!*(bool*)args[4])
        return;
    int cross = this->getCrossNum();
    if(cross > 0)
    {
        coder.askForSkill(this->getID(),"地枪");
        int reply = messageBuffer::readInfor();
        if(reply != 0)
        {
            cross-=reply;
            this->setCrossNum(cross);
            coder.crossChangeNotice(this->getID(), cross);
            coder.notice("圣枪发动【地枪】，使用"+QString::number(reply)+"点治疗");
            Harm *harm = (Harm*)args[2];
            harm->harmPoint += reply;
        }
        else
        {
            int max = this->getCrossMax();
            if(cross < max)
            {
                cross++;
                this->setCrossNum(cross);
                coder.crossChangeNotice(this->getID(), cross);
            }
            coder.notice("圣枪发动【圣击】，增加1治疗");
        }
    }
    else
    {
        int max = this->getCrossMax();
        if(cross < max)
        {
            cross++;
            this->setCrossNum(cross);
            coder.crossChangeNotice(this->getID(), cross);
        }
        coder.notice("圣枪发动【圣击】，增加1治疗");
    }

}

//圣光祈愈
void ShengQiang::ShengGuangQiYu(QList<void *> args)
{
    BatInfor* magic =(BatInfor*)args[0];
    if(magic->srcID != this->getID())
        return;
    if(magic->infor1 != 1006)
        return;
    if(getGem()==0)
        return;
    ShengGuangQiYuUsed = true;
    coder.notice("圣枪发动【圣光祈愈】，增加2治疗");
    this->gem--;
    coder.energyNotice(this->getID(),this->getGem(),this->getCrystal());
    int cross = this->getCrossNum();
    cross+=2;
    if(cross>5)
        cross=5;
    this->setCrossNum(cross);
    coder.crossChangeNotice(this->getID(), cross);
    this->engine->addActionNum(ATTACK);
}

void ShengQiang::skillReset(QList<void *> args)
{
    if(this != ((PlayerEntity*)args[0]))
        return;
    ShengGuangQiYuUsed = false;
}

void ShengQiang::makeConnection(BackgroundEngine* engine)
{
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(HuiYao(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(ChengJie(QList<void*>)));
    connect(engine,SIGNAL(skillMagic(QList<void*>)),this,SLOT(ShengGuangQiYu(QList<void*>)));
    connect(engine,SIGNAL(timeLine1SIG(QList<void*>)),this,SLOT(TianQiang(QList<void*>)));
    connect(engine,SIGNAL(timeLine2hitSIG(QList<void*>)),this,SLOT(ShengJi(QList<void*>)));
    connect(engine,SIGNAL(timeLine2hitSIG(QList<void*>)),this,SLOT(DiQiang(QList<void*>)));
    connect(engine,SIGNAL(actionPhaseSIG(QList<void*>)),this,SLOT(skillReset(QList<void*>)));
}
