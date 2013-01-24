#include "shengqiang.h"

ShengQiang::ShengQiang()
{
    makeConnection();
    Button *huiYao, *chengJie, *shengGuangQiYu;
    huiYao = new Button(3,tr("��ҫ"));
    buttonArea->addButton(huiYao);
    connect(huiYao,SIGNAL(buttonSelected(int)),this,SLOT(HuiYao()));

    chengJie = new Button(4,tr("�ͽ�"));
    buttonArea->addButton(chengJie);
    connect(chengJie,SIGNAL(buttonSelected(int)),this,SLOT(ChengJie()));

    shengGuangQiYu = new Button(5,tr("ʥ������"));
    buttonArea->addButton(shengGuangQiYu);
    connect(shengGuangQiYu,SIGNAL(buttonSelected(int)),this,SLOT(ShengGuangQiYu()));
}

void ShengQiang::normal()
{
    Role::normal();
    Player* myself=dataInterface->getMyself();
    if(handArea->checkElement("water"))
        buttonArea->enable(3);
    bool chengjieflag = false;
    QList<Player*>players=dataInterface->getPlayerList();
    for(int i=0;i<players.size();i++)
    {
        if(myself->getID()==players[i]->getID())
            continue;
        if(players[i]->getCrossNum()>0)
        {
            chengjieflag = true;
            break;
        }
    }
    if(handArea->checkType("magic")&&chengjieflag)
        buttonArea->enable(4);
    if(myself->getGem()>0)
        buttonArea->enable(5);
}

void ShengQiang::HuiYao()
{
    state=1001;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();

    handArea->setQuota(1);

    decisionArea->enable(1);
    decisionArea->disable(0);

    handArea->enableElement("water");
}

void ShengQiang::ChengJie()
{
    state=1002;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();

    playerArea->setQuota(1);
    handArea->setQuota(1);

    decisionArea->enable(1);
    decisionArea->disable(0);

    handArea->enableMagic();

}

void ShengQiang::TianQiang()
{
    state=36;
    tipArea->setMsg(tr("�Ƿ񷢶���ǹ��"));
    decisionArea->enable(0);
    decisionArea->enable(1);
}

void ShengQiang::DiQiang()
{
    state=1005;
    tipArea->setMsg(tr("��ѡ�񷢶���ǹʹ�õ����Ƶ�����ȡ��������ʥ��"));
    decisionArea->enable(0);
    decisionArea->enable(1);
    Player* myself=dataInterface->getMyself();
    int cross = myself->getCrossNum();
    if (cross>4)
        cross = 4;
    for(;cross>0;cross--)
        tipArea->addBoxItem(QString::number(cross));
    tipArea->showBox();
}

void ShengQiang::ShengGuangQiYu()
{
    state=1006;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();

    decisionArea->enable(1);
    decisionArea->enable(0);
}

void ShengQiang::cardAnalyse()
{
    Role::cardAnalyse();

    switch (state)
    {
    case 1001:
        decisionArea->enable(0);
        break;
    case 1002:
        Player* myself=dataInterface->getMyself();
        QList<Player*>players=dataInterface->getPlayerList();
        for(int i=0;i<players.size();i++)
        {
            if(myself->getID()==players[i]->getID())
                continue;
            if(players[i]->getCrossNum()>0)
                playerArea->enablePlayerItem(i);
        }
        break;
    }

}

void ShengQiang::onOkClicked()
{
    Role::onOkClicked();
    QList<Card*>selectedCards;
    QList<Player*>selectedPlayers;

    static QString command;
    QString cardID;
    QString sourceID;
    QString targetID;
    QString text;

    selectedCards=handArea->getSelectedCards();
    selectedPlayers=playerArea->getSelectedPlayers();

    switch(state)
    {
    case 42:
        text=tipArea->getBoxCurrentText();
        if(text[0]=='1'){
            actionFlag=1;
            actions.removeOne(tr("1.�����ж�(��ҫ)"));
            attackAction();
        }
        else if(text[0]=='2'){
            actionFlag=1;
            actions.removeOne(tr("2.�����ж�(�ͽ�)"));
            attackAction();
        }
        else if(text[0]=='3'){
            actionFlag=1;
            actions.removeOne(tr("3.��ڦ�ж�(ʥ������)"));
            attackAction();
        }
        break;
    case 1001:
        command="1001;";
        cardID=QString::number(selectedCards[0]->getID());
        sourceID=QString::number(myID);
        command+=cardID+";"+sourceID+";";
        dataInterface->removeHandCard(selectedCards[0]);
        emit sendCommand(command);
        gui->reset();
        break;
    case 1002:
        command="1002;";
        cardID=QString::number(selectedCards[0]->getID());
        sourceID=QString::number(myID);
        targetID=QString::number(selectedPlayers[0]->getID());
        command+=cardID+";"+targetID+";"+sourceID+";";
        dataInterface->removeHandCard(selectedCards[0]);
        emit sendCommand(command);
        gui->reset();
        break;
    case 36:
        command="36;1;";
        emit sendCommand(command);
        gui->reset();
        break;
    case 1005:
        command="1005;";
        command+=tipArea->getBoxCurrentText();
        emit sendCommand(command);
        gui->reset();
        break;
    case 1006:
        command="1006;";
        sourceID=QString::number(myID);
        command+=sourceID+";";
        emit sendCommand(command);
        gui->reset();;
        break;
    }
}

void ShengQiang::onCancelClicked()
{
    Role::onCancelClicked();
    QString command;
    switch(state)
    {
    case 36:
        command="36;0;";
        emit sendCommand(command);
        gui->reset();
        break;
    case 1005:
        command="1005;0;";
        emit sendCommand(command);
        gui->reset();
        break;
    case 1:
    case 1001:
    case 1002:
    case 1006:
        if(actionFlag==0)
            normal();
        else if(actionFlag==1)
            attackAction();
        break;
    }
}

void ShengQiang::decipher(QString command)
{
    Role::decipher(command);
    QStringList arg=command.split(';');
    int targetID;
    QString flag;

    switch (arg[0].toInt())
    {
//�ж��׶� flag 0-�����ж���1-�����ж���2-�����ж���3-�����ж���4-���������ж�
    case 29:
        targetID=arg[1].toInt();
        flag=arg[2];
        if(targetID==myID)
        {
            if(flag=="0")
                normal();
        }
        break;
//������Ӧѯ��
    case 35:
        targetID=arg[1].toInt();
        flag=arg[2];
        if(targetID==myID)
        {
            gui->setEnable(1);
            if(flag==tr("��ǹ"))
                TianQiang();
            else if(flag==tr("��ǹ"))
                DiQiang();
        }
        break;
//�����ж�ѯ��
    case 42:
        targetID=arg[1].toInt();
        if(targetID==myID)
        {
            if(state==1001){
                actions.append(tr("1.�����ж�(��ҫ)"));
            }
            if(state==1002){
                actions.append(tr("2.�����ж�(�ͽ�)"));
            }
            if(state==1006){
                actions.append(tr("3.�����ж�(ʥ������)"));
            }
            foreach(QString ptr,actions)
                tipArea->addBoxItem(ptr);
            tipArea->showBox();
            state=42;
        }
    }
}
