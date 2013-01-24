#include "BackgroundEngine.h"
#include "Characters.h"
#include "Server.h"
#include <QTextStream>
#include <QFile>

void TeamArea::initialTeam()
{
    this->moraleBLUE = 15;
    this->moraleRED = 15;
    this->gemBLUE = 0;
    this->gemRED = 0;
    this->crystalBLUE = 0;
    this->crystalRED = 0;
    this->cupBLUE = 0;
    this->cupRED = 0;
}

void TeamArea::setCrystal(int color, int value)
{
    if(color == RED && value+gemRED<=5)
        this->crystalRED = value;
    else if(color == BLUE && value+gemBLUE<=5)
        this->crystalBLUE = value;
}

void TeamArea::setCup(int color, int value)
{
    if(color == RED)
        this->cupRED = value;
    else if(color == BLUE)
        this->cupBLUE = value;
}

void TeamArea::setGem(int color, int value)
{
    if(color == RED && value+crystalRED<=5)
        this->gemRED = value;
    else if(color == BLUE && value+crystalBLUE<=5)
        this->gemBLUE = value;
}

void TeamArea::setMorale(int color, int value)
{
    if(color == RED)
        this->moraleRED = value;
    else if(color == BLUE)
        this->moraleBLUE = value;

}

BackgroundEngine::BackgroundEngine():QObject(0)
{
    this->initial();
}

PlayerEntity* BackgroundEngine::setRole(int roleID,BackgroundEngine* engine,int id,int color)
{
    switch(roleID)
    {
    case 1:
        return new JianSheng(engine,id,color);
        break;
    case 2:
        return new Berserker(engine,id,color);
        break;
    case 3:
        return new BowLady(engine,id,color);
        break;
    case 4:
        return new FengYin(engine,id,color);
        break;
    case 5:
        return new AnSha(engine,id,color);
        break;
    case 6:
        return new Saintness(engine,id,color);
        break;
    case 7:
        return new TianShi(engine,id,color);
        break;
    case 8:
        return new MoDao(engine,id,color);
        break;
    case 9:
        return new MoJian(engine,id,color);
        break;
    case 10:
        return new ShengQiang(engine, id, color);
        break;
    case 12:
        return new MaoXian(engine,id,color);
        break;
    }
}
void BackgroundEngine::randomize(QList<int> *queue)
{
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec());
    int n=queue->size();
    int choosen;
    for(int i=n-1;i>0;i--)
    {
        choosen=qrand() % i;
        queue->swap(i,choosen);
    }
}

void BackgroundEngine::seatArrange()
{
    //��λ����,�����ڼ�û�в����������,�Ժ�Ӧ�ø�Ϊ�������
    QString code="";
    QList<int> ids;
    for(int i=0;i<playerSum;i++)
        ids<<i;
    randomize(&ids);
    QList<int> roles;
    for(int i=1; i<= 9 ;i++)
        roles<<i;
    roles<<12;
    randomize(&roles);
    int colors[]={1,0,1,0,0,1};
    this->playerList.clear();
    setPlayerNum(playerSum);
    for(int i = 0;i < this->getPlayerNum();i++)
    {
        PlayerEntity* player;
        //���Թ�Ů

        player = setRole(10,this,ids[i],colors[i]);
        code+=QString::number(ids[i]);
        //PlayerEntity* player = new PlayerEntity(this,code.at(i).digitValue(),code.at(i+this->playerNum).digitValue());
        this->playerList << player;

    }
    code+="101001";
    for(int i = 0;i < this->getPlayerNum() - 1;i++)
    {
        this->playerList.at(i)->setNext(this->playerList.at(i+1));
    }
    this->playerList.at(this->getPlayerNum()-1)->setNext(this->playerList.at(0));
    emit makePlayerConnectSIG(this->getPlayerNum());

    for(int i = 0;i < this->playerList.size();i++)
    {
        this->playerList[i]->setSeatNum(i);
    }

    coder.beginNotice(code);
}
void BackgroundEngine::characterSynchron()
{
    foreach(PlayerEntity*ptr, playerList)
    {
        coder.characterNotice(ptr->getID(),ptr->getRoleID());
    }
    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//��Ϸ��ʼ,��Ϸ���̿���
void BackgroundEngine::gameStart()
{
    for(int i = 0;i < this->getPlayerNum();i++)
    {
        this->drawCards(4,NOTHARMED,this->playerList.at(i));
    }

    this->setCurrentPlayer(this->playerList.at(0));

    teamArea.initialTeam();

    this->playing = true;
    while(this->playing)
    {
       this->turnBeginPhase(this->getCurrentPlayer());

       if(!this->playing)
           break;

       this->actionPhase();

       if(!this->playing)
           break;

       this->turnEndPhase();

       if(!this->playing)
           break;

       this->setCurrentPlayer(this->getCurrentPlayer()->getNext());

       if(!this->playing)
           break;
   }
   clearData();
}
//��ձ�������
void BackgroundEngine::clearData()
{
    for(int i = 0;i < cardList.size();i++)
    {
        delete cardList[i];
    }
    cardList.clear();
    for(int i = 0;i < this->playerList.size();i++)
    {
        delete playerList[i];
    }
    this->playerList.clear();
}

//��ʼ������
void BackgroundEngine::initial()
{
    this->pile.clear();
    this->discardPile.clear();
    this->discardPileCovered.clear();
    //���뿨���ļ�
    QFile log("./log.txt");
    log.open(QIODevice::WriteOnly);
    QTextStream out(&log);

    out << "try to open ";

    QFile fp("./cardDB.txt");

    QTextStream in(&fp);
    QTextCodec *codec=QTextCodec::codecForName("GBK");
    in.setCodec(codec);

    if (!fp.open(QIODevice::ReadOnly)) {
            out << "Cannot open file for reading. ";
    }

    out << "ready to read";

    QStringList cardEntry;
    while (!in.atEnd())
    {
        cardEntry=in.readLine().split("\t");
        cardList<<new CardEntity(&cardEntry);       
    }

    out << cardList.length();
    log.close();
    //ϴ��
    this->shuffle(false);
    fp.close();

}

//����ϴ���ƶ�
void BackgroundEngine::shuffle(bool reShuffle)
{
    QFile fp("./log.txt");
    fp.open(QIODevice::WriteOnly);
    QTextStream out(&fp);
    //��ϵͳʱ�������������
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    out << "\n shuffle\n";
    fp.close();
    this->pile.clear();

    if(!reShuffle)
    {
        for(int i = cardList.size()-1;i >=0;i--)
        {
            this->pile << NULL;
        }
        int left = cardList.size();
        for(int i = 0;i < cardList.size();i++)
        {
            int order = qrand() % left;

            for(int j = 0;j < this->pile.size();j++)
            {
                if(this->pile.at(j) != NULL)
                    continue;
                if(order == 0)
                {
                    this->pile[j] = cardList.at(i);
                    this->pile.at(j)->setOwner(OWNERLESS);
                    this->pile.at(j)->setPlace(PILE);
                    left--;
                    break;
                }
                else
                    order--;
            }
        }
    }
    else
    {
        for(int i = 0;i < this->discardPile.size() + this->discardPileCovered.size();i++)
        {
            this->pile << NULL;
        }
        int left = this->discardPile.size() + this->discardPileCovered.size();
        for(int i = 0;i < this->discardPile.size();i++)
        {
            int order = qrand() % left;
            for(int j = 0;j < this->pile.size();j++)
            {
                if(this->pile.at(j) != NULL)
                    continue;
                if(order == 0)
                {
                    this->pile[j] = this->discardPile.at(i);
                    this->pile.at(j)->setOwner(OWNERLESS);
                    this->pile.at(j)->setPlace(PILE);
                    left--;
                    break;
                }
                else
                    order--;
            }
        }
        for(int i = 0;i < this->discardPileCovered.size();i++)
        {
            int order = qrand() % left;
            for(int j = 0;j < this->pile.size();j++)
            {
                if(this->pile.at(j) != NULL)
                    continue;
                if(order == 0)
                {
                    this->pile[j] = this->discardPileCovered.at(i);
                    this->pile.at(j)->setOwner(OWNERLESS);
                    this->pile.at(j)->setPlace(PILE);
                    left--;
                    break;
                }
                else
                    order--;
            }
        }
        this->discardPile.clear();
        this->discardPileCovered.clear();
        coder.reshuffleNotice();
        //emit this->sendMessageSIG(-1,combMessage(QString::number(10)));
    }
}
//����Ƿ������Ϸ
bool BackgroundEngine::checkEnd()
{
    if(teamArea.getMorale(RED) <= 0 || teamArea.getCup(BLUE) == 5)
    {
        coder.endNotice(BLUE);

        this->playing = false;
    }
    else if(teamArea.getMorale(BLUE) <= 0 || teamArea.getCup(RED) == 5)
    {
        coder.endNotice(RED);

        this->playing = false;
    }
    return !playing;
}
//���ƺ���
void BackgroundEngine::drawCards(int num,int harmed,PlayerEntity* player)
{
    QList<CardEntity*> newCards;
    for(int i = 0;i < num;i++)
    {
        if(this->pile.isEmpty())
        {
            shuffle(true);
        }
        newCards << this->pile.takeLast();
    }

    coder.drawNotice(player->getID(),num,newCards);
    //����������������
    player->addHandCards(newCards,harmed);
}
//�ж�����
void BackgroundEngine::posionProcess(PlayerEntity* player,CardEntity* card)
{
    Harm harm;
    harm.harmPoint = 1;
    harm.type = MAGIC;
    //coder.magicHurtNotice(player->getID(),1,card->getSrcUser(),"�ж�");
    this->timeLine3(harm,getPlayerByID(card->getSrcUser()),player,"�ж�");

}
//��������
void BackgroundEngine::weakProcess(PlayerEntity* player,int howMany)
{
    coder.askForWeak(player->getID(),howMany);

    int reply = messageBuffer::readInfor();


    if(reply == 0)
    {
        //����
        this->attackLeft = 0;
        this->magicLeft = 0;
        this->specialLeft = 0;
        this->attackOrMagicLeft = 0;
        this->actionLeft = 0;

        coder.weakNotice(player->getID(),0);
    }
    else if(reply == 1)
    {
        //ǿ��
        coder.weakNotice(player->getID(),1,howMany);
        this->drawCards(howMany,0,player);
    }
}

//�غϿ�ʼʱ��⵱ǰ�����ǰ�Ļ���Ч����ר��Ч��
void BackgroundEngine::checkEffect(PlayerEntity* player)
{
    for(int i = 0;i < player->getBasicEffect().size();i++)
    {
        if(player->getBasicEffect().at(i)->getMagicName() == POISONCARD)
        {
            posionProcess(player,player->getBasicEffect()[i]);
            player->removeBasicEffect(player->getBasicEffect()[i]);
            i--;
        }      
    }

    bool withoutTrap=true;
    emit weakSIG(player,&withoutTrap);
    for(int i = 0;i < player->getBasicEffect().size()&&withoutTrap;i++)
        if(player->getBasicEffect().at(i)->getMagicName() == WEAKCARD)
        { 
            weakProcess(player);
            player->removeBasicEffect(player->getBasicEffect()[i]);
            break;
        }    
}

//�ж�һ��֮��Ĵ����ۼ�����ж���ʣ���ж�����
void BackgroundEngine::acted(int kind)
{
    if(kind == ATTACK)
    {
        if(this->attackLeft != 0)
            this->attackLeft--;
        else if(this->attackOrMagicLeft != 0)
            this->attackOrMagicLeft--;
        else
            this->actionLeft--;
    }
    else if(kind == MAGIC)
    {
        if(this->magicLeft != 0)
            this->magicLeft--;
        else if(this->attackOrMagicLeft != 0)
            this->attackOrMagicLeft--;
        else
            this->actionLeft--;
    }
    else if(kind == SPECIAL)
    {
        if(this->specialLeft != 0)
            this->specialLeft--;
        else
            this->actionLeft--;
    }

//    qDebug() << attackNew<<magicNew<<attackOrMagicNew<<specialNew<<actionNew;
//    qDebug() << attackLeft<<magicLeft<<attackOrMagicLeft<<specialLeft<<actionLeft;
}

void BackgroundEngine::turnBeginPhase(PlayerEntity* currentPlayer)
{
    this->setCurrentPlayer(currentPlayer);
    this->currentPlayer->setYourTurn(true);
    this->resetActionNum();

    coder.turnBegineNotice(currentPlayer->getID());
    QList<void*> args ;
    args<<currentPlayer;
    emit turnBeginPhaseSIG(args);

    //checkEffect();
}
//���¸������ж��Ƿ���ִ����Ӧ�ж�
bool BackgroundEngine::allowAttack()
{
    if(this->actionLeft != 0 || this->attackOrMagicLeft != 0 || this->attackLeft != 0)
        return true;
    else
        return false;
}

bool BackgroundEngine::allowMagic()
{
    if(this->actionLeft != 0 || this->attackOrMagicLeft != 0 || this->magicLeft != 0)
        return true;
    else
        return false;
}

bool BackgroundEngine::allowSpecial()
{
    if(this->actionLeft != 0)
        return true;
    else
        return false;
}

void BackgroundEngine::addActionNum(int kind)
{
    switch(kind)
    {
    case ATTACK:
        this->attackLeft++;
        break;
    case MAGIC:
        this->magicLeft++;
        break;
    case ATTACKORMAGIC:
        this->attackOrMagicLeft++;
        break;
    case SPECIAL:
        this->specialLeft++;
        break;
    case ALLACTION:
        this->actionLeft++;
        break;
    }
}



//�ж��׶κ���
void BackgroundEngine::actionPhase()
{    
    //�����Ǳ����Ƿ��ж���������Ѿ��ж�������ô׷�ӵĸ����ж�������Է���
    bool acted = false;
    bool firstTime=true;

    this->checkEffect(this->currentPlayer);
    QList<void*> args;
    args<<currentPlayer;

    while((this->allowAttack()||this->allowMagic()||this->allowSpecial())&&playing)
    {        
        //����������ж����ѯ��client
        if(firstTime){
            emit actionPhaseSIG(args);
            firstTime=false;
        }
        if(!acted){
            coder.askForAction(currentPlayer->getID(),0,acted);
        }
        else
            coder.askForAdditionalAction(currentPlayer->getID());

        //��ȡclient�Ļظ�
        BatInfor bat = messageBuffer::readBatInfor();

        //���������ж�
        if(bat.reply == FINISH)
            break;
        //�޷��ж�
        if(bat.reply == UNACTIONAL)
        {                        
            //��������
            this->reDraw();
            continue;
        }

        if(bat.reply == ATTACK || bat.reply==ATTACKSKILL)
        {
            //ʹ�ù�����
            CardEntity* usingCard = getCardByID(bat.CardID);
            PlayerEntity* dst = getPlayerByID(bat.dstID);
            PlayerEntity* src = getPlayerByID(bat.srcID);
            QList<CardEntity*> use;
            int realCard=1;
            use << usingCard;
            args.clear();
            args<<&bat;
            args<<&realCard;
            if(bat.reply==ATTACKSKILL)
                emit skillAttack(args);
            this->useCard(use,src,dst,false,realCard);
            this->timeLine1(usingCard,src,dst,true);

            acted = true;
            this->acted(ATTACK);
            args.clear();
            args<< src;
            args << dst;
            emit attackFinishSIG(args);

        }

        else if(bat.reply == MAGIC)
        {
            //ʹ�÷�����
            if(bat.infor1 == COMMONMAGIC)
                this->useMagicCard(bat.CardID,bat.srcID,bat.dstID);
            else
            {
                QList<void*> args;
                args << &bat;
                emit this->skillMagic(args);
            }
            acted = true;
            this->acted(MAGIC);
            PlayerEntity* src = getPlayerByID(bat.srcID);
            args.clear();
            args<< src;
            emit magicFinishSIG(args);
        }
        else if(bat.reply == SPECIAL)
        {
            //�����ж�
            if(bat.CardID == BUY)
            {
                coder.notice("ִ�С�����");
                this->drawCards(3,0,this->getCurrentPlayer());
                int color = this->currentPlayer->getColor();
                teamArea.setGem(color,teamArea.getGem(color) + bat.infor1);
                teamArea.setCrystal(color,teamArea.getCrystal(color) + bat.infor2);                
                coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));
            }
            else if(bat.CardID == SYNTHESIZE)
            {
                coder.notice("ִ�С��ϳɡ�");
                this->drawCards(3,0,currentPlayer);
                int color = this->currentPlayer->getColor();
                teamArea.setGem(color,teamArea.getGem(color) - bat.infor1);
                teamArea.setCrystal(color,teamArea.getCrystal(color) - bat.infor2);
                teamArea.setCup(color,teamArea.getCup(color) + 1);
                teamArea.setMorale(!color,teamArea.getMorale(!color) - 1);                
                coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));
                coder.cupNotice(color,teamArea.getCup(color));
                coder.moraleNotice(!color,teamArea.getMorale(!color));
                this->checkEnd();
            }
            else if(bat.CardID == EXTRACT)
            {
                coder.notice("ִ�С�������");
                int color = this->currentPlayer->getColor();
                teamArea.setGem(color,teamArea.getGem(color) - bat.infor1);
                teamArea.setCrystal(color,teamArea.getCrystal(color) - bat.infor2);
                currentPlayer->setGem(currentPlayer->getGem()+bat.infor1);
                currentPlayer->setCrystal(currentPlayer->getCrystal()+bat.infor2);                
                coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));
                coder.energyNotice(currentPlayer->getID(),currentPlayer->getGem(),currentPlayer->getCrystal());
            }
            else {
                QList<void*> args;
                args << &bat;
                emit skillSpecial(args);
            }
            acted = true;
            this->acted(SPECIAL);
        }
    }
}

void BackgroundEngine::turnEndPhase()
{
    //emit turnEndPhaseSIG();

}

//�ж���ǰ�Ƿ�����ж�����4ʥ��������
//�˺���δʹ�ã��ù��ܽ���client���
bool BackgroundEngine::canAct()
{
    if(this->currentPlayer->getHandCards().length() <= this->currentPlayer->getHandCardMax() - 3)
        return true;
    for(int i = 0;i < this->currentPlayer->getHandCards().length();i++)
    {
        if(this->currentPlayer->getHandCards().at(i)->getName() != tr("ʥ��"))
            return true;
    }
    return false;
}

void BackgroundEngine::reDraw()
{
    coder.unactionalNotice(this->currentPlayer->getID());
    int cardNum = this->currentPlayer->getHandCards().length();
    coder.discardNotice(this->currentPlayer->getID(),cardNum,"y",this->currentPlayer->getHandCards());
    this->currentPlayer->removeHandCards(this->getCurrentPlayer()->getHandCards(),true,true);    
    this->drawCards(cardNum,0,this->currentPlayer);
}

//���û���Ч����ĳ�����ǰ
void BackgroundEngine::effectApply(CardEntity* card,PlayerEntity* user,PlayerEntity* dst)
{
    QList<CardEntity*> cards;
    cards << card;
    int userID=user->getID();
    int dstID=dst->getID();

    card->setSrcUser(userID);
    card->setOwner(dstID);
    card->setPlace(EFFECT);
    user->removeHandCards(cards,true,false);
    coder.moveCardNotice(1,cards,userID,HAND,dstID,EFFECT);
    dst->addBasicEffect(card);
    if(card->getName()==tr("ʥ��")||card->getSpecialityList().contains(tr("��ʹ֮ǽ")))
        emit usedShield(userID);

}

//"ʹ��"����ʱ���ô˺���
//���ĸ�������������ʹ�ú��Ƿ����ڳ��ϣ�����Ч����
void BackgroundEngine::useCard(QList<CardEntity*> cards,PlayerEntity* user,PlayerEntity* dst,bool stay,int realCard)
{    
    coder.useCardNotice(cards,(dst == NULL)?-1:dst->getID(),user->getID(),realCard);
    if(!realCard)
        return;
    if(!stay)
        user->removeHandCards(cards,true);
    else
    {
        this->effectApply(cards[0],user,dst);
    }    
}

void BackgroundEngine::useMagicCard(int cardID, int srcID, int dstID)
{
    CardEntity *magicCard = getCardByID(cardID);
    PlayerEntity *srcPlayer = this->getPlayerByID(srcID);
    PlayerEntity *dstPlayer = this->getPlayerByID(dstID);
    QList<CardEntity*> cards;
    cards << magicCard;


    if(magicCard->getMagicName() == POISONCARD)
    {   
        this->useCard(cards,srcPlayer,dstPlayer,true);

    }
    else if(magicCard->getMagicName() == SHIELDCARD)
    {
        this->useCard(cards,srcPlayer,dstPlayer,true);

    }
    else if(magicCard->getMagicName() == WEAKCARD)
    {
        this->useCard(cards,srcPlayer,dstPlayer,true);

    }
    else if(magicCard->getMagicName() == MISSILECARD)
    {
        this->useCard(cards,srcPlayer,dstPlayer);
        this->missileProcess(magicCard,srcID,dstID);
    }
}

//����Ϊʱ���ắ��
void BackgroundEngine::timeLine1(CardEntity* attackCard,PlayerEntity* src,PlayerEntity* dst,bool isActiveAttack)
{   
    Harm harm = getHarmFromCard(attackCard);
    int attackType = (attackCard->getElement() == "darkness")?NOREPLY:NORMAL;
    QList<void*> args;
    args << src;
    args << dst;
    args << &harm;
    args << attackCard;
    args << &isActiveAttack;
    args << &attackType;
    emit timeLine1ProSIG(args);//���ź��������˺�ʱ����֮ǰ��Ч�ļ���
    emit timeLine1SIG(args);
    timeLine2(attackCard,src,dst,isActiveAttack,attackType,harm);
}

void BackgroundEngine::timeLine2(CardEntity* harmCard,PlayerEntity* src,PlayerEntity* dst,bool isActiveAttack,int attackType,Harm harm)
{
    coder.askForReBat(attackType,harmCard->getID(),dst->getID(),src->getID());
    //�������Ǳ���ʱ�������ܿͻ��˻�Ӧ
    //emit askForReply(dst->getHandCards(),element,dst->getID());
    BatInfor temp;
    bool checkShield=true;
    if(attackType != NOMISS)
    {
        //ս����Ϣ�洢��temp�С���Ӧս���򱣴���Ӧս���ƣ����ֵ����򱣴���ʥ���ʥ��
        temp = messageBuffer::readBatInfor();
    }
    else
    {
        temp.reply = HIT;
        checkShield=false;
    }

    if(temp.reply == REPLYBATTLE)
    {
        //Ӧս
        //�ƺ������źŵ�ʱ����Ҫ��ȶ
        CardEntity* usedCard = getCardByID(temp.CardID);
        QList<CardEntity*> use;
        use<<usedCard;
        this->useCard(use,getPlayerByID(temp.srcID),getPlayerByID(temp.dstID));

        coder.hitNotice(BLOCKED,isActiveAttack,dst->getID(),src->getID());
        QList<void*> args;
        args << src;
        args << dst;
        args << &harm;
        args << harmCard;
        args << &isActiveAttack;
        emit timeLine2missedSIG(args);
        this->timeLine1(getCardByID(temp.CardID),getPlayerByID(temp.srcID),getPlayerByID(temp.dstID),false);
    }
    else if(temp.reply == BLOCKED)
    {
        //�ֵ�
        CardEntity* usedCard = getCardByID(temp.CardID);

        if(usedCard->getPlace() == HAND)
        {
            //ʥ��
            QList<CardEntity*> use;
            use<<usedCard;
            this->useCard(use,getPlayerByID(temp.srcID));
        }
        coder.hitNotice(BLOCKED,isActiveAttack,dst->getID(),src->getID());
        QList<void*> args;
        args << src;
        args << dst;
        args << &harm;
        args << harmCard;
        args << &isActiveAttack;
        emit timeLine2missedSIG(args);

    }
    else if(temp.reply == HIT)
    {    
        //���е����

        QList<void*> args;
        args << src;
        args << dst;
        args << &checkShield;
        emit shieldSIG(args);
        for(int i = 0;i < dst->getBasicEffect().size()&& checkShield;i++)
        {
            //����Ƿ���ʥ��                        
            if(dst->getBasicEffect().at(i)->getMagicName() == SHIELDCARD || dst->getBasicEffect().at(i)->getSpecialityList().contains(tr("��ʹ֮ǽ")))
            {
                coder.shieldNotic(dst->getID());
                dst->removeBasicEffect(dst->getBasicEffect()[i]);

                coder.hitNotice(BLOCKED,isActiveAttack,dst->getID(),src->getID());
                args.clear();
                args << src;
                args << dst;
                args << &harm;
                args << harmCard;
                args << &isActiveAttack;
                emit timeLine2missedSIG(args);
                return;
            }
        }
        //emit timeLine2hitSIG();

        coder.hitNotice(HIT,isActiveAttack,dst->getID(),src->getID());
        args.clear();
        args << src;
        args << dst;
        args << &harm;
        args << harmCard;
        args << &isActiveAttack;
        emit timeLine2hitSIG(args);

        int color = src->getColor();
        //������ʯ
        if((teamArea.getCrystal(color) + teamArea.getGem(color)) < 5)
        {
            if(isActiveAttack)
                teamArea.setGem(color,teamArea.getGem(color) + 1);
            else
                teamArea.setCrystal(color,teamArea.getCrystal(color) + 1);
            coder.stoneNotice(color,teamArea.getGem(color),teamArea.getCrystal(color));

        }

        this->timeLine3(harm,src,dst);
    }
}
//�ӿ����л�ȡ�����˺�
Harm BackgroundEngine::getHarmFromCard(CardEntity* card)
{
    Harm harm;
    if(card->getType() == "attack")
    {
        //harm.element = card->getElement();
        harm.harmPoint = 2;
        harm.type = ATTACK;
    }
    return harm;
}

void BackgroundEngine::timeLine3(Harm harm, PlayerEntity *src, PlayerEntity *dst,QString magicReason)
{
    QList<void*> arg;
    arg << src;
    arg << dst;
    arg << &harm;

    emit timeLine3SIG(arg);
    if(harm.type == ATTACK)
        coder.attackHurtNotice(dst->getID(),harm.harmPoint,src->getID());
    else if(harm.type == MAGIC)
        coder.magicHurtNotice(dst->getID(),harm.harmPoint,src->getID(),magicReason);
    timeLine4(harm,src,dst);
}

void BackgroundEngine::timeLine4(Harm harm,PlayerEntity *src,PlayerEntity *dst)
{
    int crossUsed = 0;
    if(dst->getCrossNum() != 0)
    {
        coder.askForCross(dst->getID(),harm.harmPoint,harm.type);
        crossUsed = messageBuffer::readInfor();
    }

    timeLine5(harm,src,dst,crossUsed);
}

void BackgroundEngine::timeLine5(Harm harm,PlayerEntity *src,PlayerEntity *dst,int cross)
{
    if(cross>0)
    {
        harm.harmPoint -= cross;
        int newCross=dst->getCrossNum()-cross;
        dst->setCrossNum(newCross);
        coder.crossChangeNotice(dst->getID(),newCross);
    }
    if(harm.harmPoint == 0)
        return;
    //emit timeLine5SIG();

    timeLine6(harm,src,dst);
}

void BackgroundEngine::timeLine6(Harm harm,PlayerEntity *src,PlayerEntity *dst)
{
    QList<void*> arg;
    arg << src;
    arg << dst;
    arg << &harm;
    emit timeLine6SIG(arg);
    if(harm.harmPoint == 0)
        return;
    this->drawCards(harm.harmPoint,harm.type,dst);
    emit timeLine6DrawedSIG(arg);
}
//���Ҷ�ӦID�����
PlayerEntity* BackgroundEngine::getPlayerByID(int ID)
{
    PlayerEntity* one;
    //���޸ģ�iӦ��С��ʵ�������
    for(int i = 0;i < this->playerNum;i++)
    {
        one = this->playerList.at(i);
        if(ID == one->getID())
            return one;      
    }
    return NULL;
}
//������
void BackgroundEngine::showTest()
{
    PlayerEntity* player;
    char temp[500];
    QString tempStr;
    for(int i = 0;i < this->getPlayerNum();i++)
    {
        player = this->playerList.at(i);
        sprintf(temp,"player%d:\n",player->getID());
        tempStr.append(temp);

        for(int j = 0;j < player->getHandCardNum();j++)
        {

            tempStr.append(player->getHandCards().at(j)->getName());
            tempStr += QString::number(player->getHandCards().at(j)->getID());
            tempStr += " ";
        }
        tempStr += "\n";
    }

    sprintf(temp,"red team:%d\n",teamArea.getMorale(RED));
    tempStr.append(temp);
    sprintf(temp,"blue team:%d\n",teamArea.getMorale(BLUE));
    tempStr.append(temp);
    emit this->toInforDisplay(tempStr);
}
//��ȡ�ϼ�
PlayerEntity* BackgroundEngine::getFront(PlayerEntity* player)
{
    int seat;
    seat = player->getSeatNum();
    seat--;
    if(seat < 0)
        seat = this->playerList.size() - 1;
    return this->playerList[seat];
}
//ħ������
void BackgroundEngine::missileProcess(CardEntity* card,int src,int dst)
{
    bool rightOrder;
    BatInfor reply;
    PlayerEntity* nextOpponent;

    //ȷ�����ݷ���
    nextOpponent = this->getCurrentPlayer()->getNext();
    while(nextOpponent->getColor() == this->currentPlayer->getColor())
        nextOpponent = nextOpponent->getNext();
    if(nextOpponent->getID() == dst)
        rightOrder = true;
    else
        rightOrder = false;

    bool passed[6];
    for(int i = 0;i < this->playerList.size();i++)
    {
        passed[i] = false;
    }
    int missilePoint = 2;

    QList<CardEntity*> cards;


    do
    {
        cards.clear();
        //ħ�����ݵ��¼�
        missilePass(rightOrder,dst,src,passed,missilePoint);

        //��ȡ�û��ظ�
        reply = messageBuffer::readBatInfor();

        if(reply.reply == 0)
        {
            //��������
            src = dst;
            dst = reply.dstID;
            missilePoint++;
            cards << getCardByID(reply.CardID);
            this->useCard(cards,getPlayerByID(src),getPlayerByID(dst),false);
            continue;
        }
        else if(reply.reply == 1)
        {
            //ʥ��
            cards << getCardByID(reply.CardID);
            this->useCard(cards,getPlayerByID(dst));
            break;
        }
        else if(reply.reply == 2)
        {
            //��Ӧ��
            PlayerEntity* dstPlayer = getPlayerByID(dst);
            bool shieldBlocked = false;
            //���ʥ��
            for(int i = 0;i < dstPlayer->getBasicEffect().size();i++)
            {
                if(dstPlayer->getBasicEffect()[i]->getMagicName() == SHIELDCARD||dstPlayer->getBasicEffect().at(i)->getSpecialityList().contains(tr("��ʹ֮ǽ")))
                {
                    coder.shieldNotic(dst);
                    dstPlayer->removeBasicEffect(dstPlayer->getBasicEffect()[i]);
                    shieldBlocked = true;
                    break;
                }
            }
            if(shieldBlocked)
                break;
            Harm missileHurt;
            missileHurt.harmPoint = missilePoint;
            missileHurt.type = MAGIC;
            //��ʥ��,����˺�
            this->timeLine3(missileHurt,getPlayerByID(src),dstPlayer,"ħ��");
            break;
        }
        else if(reply.reply == 802)
        {
            //��������
            src = dst;
            dst = reply.dstID;
            missilePoint++;
            cards << getCardByID(reply.CardID);
            useCard(cards,getPlayerByID(src),getPlayerByID(dst));
            coder.notice("ħ��ʦ������ħ���ںϡ�");
            continue;
        }

    }while(1);

}

//ħ������
void BackgroundEngine::missilePass(bool rightOrder,int dst,int src,bool* passed,int missilePoint)
{
    int i;
    PlayerEntity* srcPlayer = getPlayerByID(src);
    PlayerEntity* dstPlayer = getPlayerByID(dst);
    PlayerEntity* next = dstPlayer;
    passed[srcPlayer->getSeatNum()] = true;

    for(i = 0;i < this->playerList.size();i++)
    {
        if(!*(passed+i) && (i != dstPlayer->getSeatNum()))
            break;
    }
    if(i == this->playerList.size())
    {
        for(int j = 0;j < this->playerList.size();j++)
            *(passed + j) = false;
    }

    if(rightOrder)
    {   
        while((next->getColor() == dstPlayer->getColor()) || *(passed + next->getSeatNum()))
            next = next->getNext();

        coder.askForMissile(dst,src,missilePoint,next->getID());
    }
    else
    {
        while((next->getColor() == dstPlayer->getColor()) || passed[next->getSeatNum()])
            next = this->getFront(next);

        coder.askForMissile(dst,src,missilePoint,next->getID());
    }
}
//���ո�ģ���ƶ����Ƶ����ƶѵ��ź�
void BackgroundEngine::toDiscardPileSLOT(QList<CardEntity*> cards,bool show)
{
    for(int i = 0;i < cards.size();i++)
    {
        if(show)
            this->discardPile << cards.at(i);
        else
            this->discardPileCovered << cards.at(i);
    }
}
