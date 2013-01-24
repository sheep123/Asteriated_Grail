#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "PlayerEntity.h"
class BackgroundEntity;
//��սʿ
class Berserker :public PlayerEntity
{
    Q_OBJECT
public :
    Berserker(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    //��
    void wild(QList<void*> args);
    //˺��
    void tear(QList<void*> args);
    //ѪӰ��
    void wildBlade(QList<void*> args);
    void wildBladeAsk(QList<void*> args);
    //Ѫ������
    void roar(QList<void*> args);
signals:
private:
    bool wildBladeUsed;
};

//��֮Ů��
class BowLady : public PlayerEntity
{
    Q_OBJECT
public :
    BowLady(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    //�����
    void thunderArrow(QList<void*> args);
    //�ᴩ���
    void transfixtion(QList<void*> args);
    //��������
    void trap(QList<void*> args);
    //��׼���
    void accurateShoot(QList<void*> args);
    //�ѻ�
    void snipe(QList<void*> args);
signals:
private:

};

class MoDao : public PlayerEntity
{
    Q_OBJECT
public :
    MoDao(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    void MoDanRongHe(QList<void*> args);
    void MoBaoChongJi(QList<void*> args);
    void HuiMeiFengBao(QList<void*> args);
signals:
private:
    \
};

class AnSha : public PlayerEntity
{
    Q_OBJECT
public :
    AnSha(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    void ShuiYing(QList<void*> args);
    void QianXing(QList<void*> args);
    void FanShi(QList<void*> args);
    void AttackCheck(QList<void*> args);
signals:
private:
    \
};

class TianShi : public PlayerEntity
{
    Q_OBJECT
public :
    TianShi(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    void FengZhiJieJin(QList<void*> args);
    void TianShiZhuFu(QList<void*> args);
    void TianShiZhiQiang(QList<void*> args);
    void TianShiZhiGe(QList<void*> args);
    void ShenZhiBiHu(int harmed,int*howMany,PlayerEntity*dst);
    void TianShiJiBan(int userID);
signals:
private:
    \
};
class Saintness : public PlayerEntity
{
    Q_OBJECT
public:
    Saintness(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine *engine);
public slots:
    //��˪����
    void prayerOfFrost(QList<void*> args);
    //������
    void cure(QList<void*> args);
    //����֮��
    void healingLight(QList<void*> args);
    //����
    void mercy(QList<void*> args);
    //ʥ��
    void layOnHands(QList<void*> args);
private:
    bool layOnHandsUsed;
};

//��ʥ
class JianSheng :public PlayerEntity
{
    Q_OBJECT
public :
    JianSheng(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    //�ҷ缼
    void LieFengJi1(QList<void*> args);
    void LieFengJi2(QList<void*> args);
    //���缼
    void JiFengJi(QList<void*> args);
    //������
    void LianXuJi1(QList<void*> args);
    void LianXuJi2(QList<void*> args);
    //��Ӱ
    void JianYing1(QList<void*> args);
    void JianYing2(QList<void*> args);
    //ʥ��
    void ShengJian(QList<void*> args);
    void skillReset(QList<void*> args);

private:
    int attackCount;
    bool LieFengJiUsed,LianXuJiUsed,JianYingUsed;
};

//��ӡ
class FengYin : public PlayerEntity
{
    Q_OBJECT
public :
    FengYin(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    //��������
    void FaShuJiDang1(QList<void*> args);
    void FaShuJiDang2(QList<void*> args);
    //��ӡ����
    void FengYinFaShu1(QList<void*> args);
    void FengYinFaShu2(QList<CardEntity *> cards, PlayerEntity *user);
    //��ϵ����
    void WuXiShuFu1(QList<void*> args);
    void WuXiShuFu2(PlayerEntity*,bool*);
    //��ӡ����
    void FengYinPoSui(QList<void*> args);

private:
    int shuFuID;
};

//ħ��
class MoJian :public PlayerEntity
{
    Q_OBJECT
public :
    MoJian(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    //�ڰ����
    void HeiAnZhenChan1(QList<void*> args);
    void HeiAnZhenChan2(QList<void*> args);
    //������ն
    void XiuLuoLianZhan1(QList<void*> args);
    void XiuLuoLianZhan2(QList<void*> args);
    //��Ӱ����
    void AnYingLiuXing(QList<void*> args);
    //��Ӱ����
    void AnYingNingJu(QList<void*> args);
    //��Ӱ֮��
    void AnYingZhiLi(QList<void*> args);
    void skillReset(QList<void*> args);
private:
    bool HeiAnZhenChanUsed,XiuLuoLianZhanUsed;
};
//ð��
class MaoXian : public PlayerEntity
{
    Q_OBJECT
public :
    MaoXian(BackgroundEngine* engine,int id,int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    //��թ
    void QiZha(QList<void*> args);
    //͵�컻��
    void TouTianHuanRi(QList<void*> args);
    //����ӹ�
    void TeShuJiaGong(QList<void*> args);
    //ð��������
    void MaoXianZheTianTang(QList<void*> args);
signals:
private:
    \
};
//ʥǹ
class ShengQiang: public PlayerEntity
{
    Q_OBJECT
public:
    ShengQiang(BackgroundEngine* engine, int id, int color);
    void makeConnection(BackgroundEngine* engine);
public slots:
    void HuiYao(QList<void*> args);
    void ChengJie(QList<void*> args);
    void ShengJi(QList<void*> args);
    void TianQiang(QList<void*> args);
    void DiQiang(QList<void*> args);
    void ShengGuangQiYu(QList<void*> args);
    void skillReset(QList<void*> args);
private:
    bool ShengGuangQiYuUsed;
};

#endif // CHARACTERS_H
