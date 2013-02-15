#include "MoGong.h"

MoGong::MoGong()
{
    makeConnection();
    setMyRole(this);

    Button *moGuangChongJi, *leiGuangSanShe, *checkCover;
    moGuangChongJi=new Button(3,"ħ����");
    buttonArea->addButton(moGuangChongJi);
    connect(moGuangChongJi,SIGNAL(buttonSelected(int)),this,SLOT(MoGuanChongJi()));
    leiGuangSanShe=new Button(4,"�׹�ɢ��");
    buttonArea->addButton(leiGuangSanShe);
    connect(leiGuangSanShe,SIGNAL(buttonSelected(int)),this,SLOT(LeiGuangSanShe()));

    checkCover = new Button(10,tr("�鿴����"));
    buttonArea->addOutsideTurnButton(checkCover);
    checkCover->setVisible(true);
    checkCover->setEnabled(true);
    connect(checkCover,SIGNAL(buttonSelected(int)),gui,SLOT(showCoverArea()));
    connect(checkCover,SIGNAL(buttonUnselected(int)),gui,SLOT(closeCoverArea()));
}

void MoGong::normal()
{
    Role::normal();
    bool moguan = false;
    bool enemyable = false;
    Player*myself = dataInterface->getMyself();
    foreach(Card*ptr,dataInterface->getCoverCards())
    {
        if(ptr->getElement()=="fire")
            moguan = true;
        if(ptr->getElement()=="thunder")
            buttonArea->enable(4);
    }
    foreach(Player*ptr,dataInterface->getPlayerList())
    {
        if(ptr->getColor()!=myself->getColor()&&ptr->getHandCardNum()<ptr->getHandCardMax())
            enemyable = true;
    }
    if(!ChongNengUsed && !DuoChongSheJiUsed && moguan && enemyable)
        buttonArea->enable(3);
    if(ChongNengUsed)
        buttonArea->disable(4);
    unactionalCheck();
}

void MoGong::MoGuanChongJi()
{
    state=2601;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    gui->showCoverArea(true);
    coverArea->reset();
    coverArea->enableElement("fire");
    coverArea->setQuota(1);
    buttonArea->enable(1);
}

void MoGong::MoGuanChongJiHit()
{
    state = 2602;
    gui->showCoverArea(true);
    coverArea->reset();
    coverArea->enableElement("fire");
    coverArea->setQuota(1);
    buttonArea->enable(1);
}

void MoGong::LeiGuangSanShe()
{
    state = 2603;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    gui->showCoverArea(true);
    coverArea->reset();
    coverArea->enableElement("thunder");
    int n = 0;
    foreach(Card*ptr,dataInterface->getCoverCards())
        if(ptr->getElement()=="thunder")
            n++;
    coverArea->setQuota(1,n);
}

void MoGong::DuoChongSheJi()
{
    state = 2605;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    gui->showCoverArea(true);
    coverArea->reset();
    coverArea->enableElement("wind");
    coverArea->setQuota(1);
}

void MoGong::ChongNengMoYan1()
{
    state = 2661;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    tipArea->setMsg("�Ƿ񷢶�����/ħ�ۣ�");
    Player* myself = dataInterface->getMyself();
    if(myself->getEnergy()>0)
        tipArea->addBoxItem("1.����");
    if(myself->getGem()>0)
        tipArea->addBoxItem("2.ħ��");
    tipArea->showBox();
    decisionArea->enable(1);
    if(myself->getEnergy()>0)
        decisionArea->enable(0);
}

void MoGong::ChongNengMoYan2()
{
    state = 2662;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    Player* myself = dataInterface->getMyself();
    if(startChoice==1)
    {
        tipArea->setMsg("����4�����ƣ���ѡ����������");
        for(int i = 1; i < 5; i ++)
            tipArea->addBoxItem(QString::number(i));
        tipArea->showBox();
        if(dataInterface->getHandCards().size()<=4)
            decisionArea->enable(0);
        else
        {
            handArea->enableAll();
            handArea->setQuota(myself->getHandCardNum()-4);
        }
        decisionArea->enable(1);
    }
    else
    {
        tipArea->setMsg("��ѡ��Ŀ���ɫ����1���ƻ򡾲�ѡ��Ŀ�꡿����3����");
        playerArea->enableAll();
        playerArea->setQuota(1);
        decisionArea->enable(0);
        decisionArea->enable(1);
    }
}

void MoGong::ChongNengGaiPai()
{
    state = 2607;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    tipArea->setMsg("��ѡ�����"+QString::number(chongnengNum)+"�����Ƹ�������");
    handArea->enableAll();
    handArea->setQuota(0,chongnengNum);
}

void MoGong::MoYanGaiPai()
{
    state = 2608;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();
    tipArea->setMsg(("��ѡ��һ�����Ƹ�������"));
    handArea->enableAll();
    handArea->setQuota(1);
    if(dataInterface->getHandCards().size()==0)
        decisionArea->enable(0);
}

void MoGong::cardAnalyse()
{
    Role::cardAnalyse();
    Player*myself=dataInterface->getMyself();
    switch(state)
    {
    case 2601:
        foreach(Player*ptr,dataInterface->getPlayerList())
        {
            if(ptr->getColor()!=myself->getColor()&&ptr->getHandCardNum()<ptr->getHandCardMax())
                playerArea->enablePlayerItem(ptr->getID());
        }
        playerArea->setQuota(1);
        break;
    case 2662:
        if(startChoice==1)
            decisionArea->enable(0);
        break;
    case 2607:
    case 2608:
        decisionArea->enable(0);
        break;
    }
}

void MoGong::coverCardAnalyse()
{
    Role::coverCardAnalyse();
    QList<Card*> selectedCoverCards = this->coverArea->getSelectedCards();
    switch(state)
    {
    case 2601:
        coverArea->reset();
        gui->showCoverArea(false);
        handArea->enableAttack();
        handArea->setQuota(1);
        break;
    case 2602:
        decisionArea->enable(0);
        break;
    case 2603:
        if(selectedCoverCards.size()==1)
            decisionArea->enable(0);
        else
        {
            playerArea->enableEnemy();
            playerArea->setQuota(1);
        }
        break;
    case 2605:
        playerArea->enableEnemy();
        playerArea->disablePlayerItem(lastTarget);
        break;
    }
}

void MoGong::onOkClicked()
{
    Role::onOkClicked();
    static QString command;
    QString sourceID = QString::number(myID);
    QString targetID;
    QString text,cardID;
    Player* myself = dataInterface->getMyself();
    QList<Card*> selectedCards = handArea->getSelectedCards();
    QList<Card*> selectedCoverCards = coverArea->getSelectedCards();
    QList<Player*> selectedPlayers = playerArea->getSelectedPlayers();
    switch(state)
    {
//NORMALACTION
    case 1:
//׷���ж�
    case 10:
    case 11:
    case 12:
        if(selectedCards.size()>0&&selectedCards[0]->getType()=="attack")
            lastTarget = selectedPlayers[0]->getID();
        break;
//�����ж�ѯ��
    case 42:
        text=tipArea->getBoxCurrentText();
        switch (text[0].digitValue()){
            emit sendCommand("2604;"+QString::number(myID)+";");
            DuoChongSheJi();
            break;
        }
        break;
    case 2601:
        MoGuanChongJiUsed = true;
        cardID=QString::number(selectedCards[0]->getID());
        emit sendCommand("1");
        targetID=QString::number(selectedPlayers[0]->getID());
        emit sendCommand("2");
        sourceID=QString::number(myID);
        emit sendCommand("3");
        command="2601;"+cardID+";"+targetID+";"+sourceID+";"+QString::number(selectedCoverCards[0]->getID())+";";
        lastTarget = selectedPlayers[0]->getID();
        usedAttack=true;
        usedMagic=usedSpecial=false;
        dataInterface->removeHandCard(selectedCards[0]);
        gui->reset();
        emit sendCommand(command);
        break;
    case 2602:
        cardID=QString::number(selectedCoverCards[0]->getID());
        command="2602;1;"+cardID+";";
        coverArea->reset();
        gui->showCoverArea(false);
        gui->reset();
        emit sendCommand(command);
        break;
    case 2603:
        if(selectedCards.size()!=0)
            targetID=QString::number(selectedPlayers[0]->getID());
        else
            targetID=QString::number(-1);
        sourceID=QString::number(myID);
        command="2603;"+targetID+";"+sourceID+";"+QString::number(selectedCoverCards.size());
        foreach(Card*ptr,selectedCoverCards)
            command+=QString::number(ptr->getID())+":";
        command+=";";
        coverArea->reset();
        gui->showCoverArea(false);
        gui->reset();
        emit sendCommand(command);
        break;
    case 2605:
        DuoChongSheJiUsed = true;
        cardID=QString::number(39);
        targetID=QString::number(selectedPlayers[0]->getID());
        sourceID=QString::number(myID);
        command="2605;"+cardID+";"+targetID+";"+sourceID+";";
        coverArea->reset();
        gui->showCoverArea(false);
        gui->reset();
        emit sendCommand(command);
        break;
    case 2661:
        startChoice = tipArea->getBoxCurrentText()[0].digitValue();
        ChongNengMoYan2();
        break;
    case 2662:
        command = "2606;";
        start = true;
        if(startChoice==1)
        {
            command+="1;";
            if(myself->getCoverCardNum()<=4)
                command+="0;";
            else
                command+=QString::number(myself->getCoverCardNum()-4);
            foreach(Card* ptr, selectedCards)
            {
                command+=QString::number(ptr->getID())+":";
                dataInterface->removeHandCard(ptr);
            }
            command+=";"+QString::number(tipArea->getBoxCurrentText()[0].digitValue())+";"+sourceID+";";
            chongnengNum = tipArea->getBoxCurrentText()[0].digitValue();
            ChongNengUsed = true;
        }
        else
        {
            command+="2;";
            if(selectedPlayers.size()>0)
                command+=QString::number(selectedPlayers[0]->getID())+";";
            else
                command+=QString::number(-1)+";";
            command+=sourceID+";";
        }
        emit sendCommand(command);
        gui->reset();
        break;
    case 2607:
        command="2607;"+QString::number(selectedCards.size())+";";
        foreach(Card* ptr, selectedCards)
        {
            command+=QString::number(ptr->getID())+";";
            dataInterface->removeHandCard(ptr);
        }
        emit sendCommand(command);
        gui->reset();
        break;
    case 2608:
        if(selectedCards.size()==0)
            command="2608;-1;";
        else
        {
            command="2608;"+QString::number(selectedCards[0]->getID())+";";
            dataInterface->removeHandCard(selectedCards[0]);
        }
        emit sendCommand(command);
        gui->reset();
        break;
    }
}

void MoGong::onCancelClicked()
{
    Role::onCancelClicked();
    static QString command;
    switch(state)
    {
    case 2601:
    case 2603:
        normal();
        break;
    case 2602:
        command = "2602;0;-1;";
        coverArea->reset();
        gui->showCoverArea(false);
        gui->reset();
        emit sendCommand(command);
        break;
    case 2605:
        DuoChongSheJi();
        break;
    case 2661:
        command = "2606;0;";
        emit sendCommand(command);
        break;
    case 2662:
        ChongNengMoYan1();
        break;
    }
}

void MoGong::attackAction()
{
    Role::attackAction();
    bool moguan = false;
    bool enemyable = false;
    Player*myself = dataInterface->getMyself();
    foreach(Card*ptr,dataInterface->getCoverCards())
        if(ptr->getElement()=="fire")
            moguan = true;
    foreach(Player*ptr,dataInterface->getPlayerList())
    {
        if(ptr->getColor()!=myself->getColor()&&ptr->getHandCardNum()<ptr->getHandCardMax())
            enemyable = true;
    }
    if(!ChongNengUsed && !DuoChongSheJiUsed && moguan && enemyable)
        buttonArea->enable(3);
}

void MoGong::additionalAction()
{
    Role::additionalAction();
    if(!MoGuanChongJiUsed&&dataInterface->getCoverCards().size()>0)
        tipArea->addBoxItem(tr("1.�������"));
}

void MoGong::turnBegin()
{
    Role::turnBegin();
    MoGuanChongJiUsed = false;
    DuoChongSheJiUsed = false;
    ChongNengUsed = false;
}

void MoGong::askForSkill(QString skill)
{
    Role::askForSkill(skill);
    if(skill==tr("����/ħ��"))
        ChongNengMoYan1();
    else if(skill==tr("���ܸ���"))
        ChongNengGaiPai();
    else if(skill==tr("ħ�۸���"))
        MoYanGaiPai();
}