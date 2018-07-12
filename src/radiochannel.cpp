#include "radiochannel.h"
#include "radpatterndata.h"

////////////////////////////////////////////////////////////////////////////////

Transceiver::Transceiver(QObject* parent, QString name, QVector3D pos) : QObject(parent)
  , m_name(name)
  , m_pos(pos)
  , m_next_event_time(0)
  , m_trx_state(trx_off)
  , m_last_trx_state(trx_off)
  , state_change_callback(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////


CSMANode::CSMANode(QObject* parent, QString name, QVector3D pos, channel chan, bool class_n, RadPatternSet* antenna) :
    Transceiver(parent, name, pos)
  , m_backoff_level(-50)
  , m_max_retries(5)
  , m_slot_time(150E-6)
  , m_max_backoff_exponent(5)
{
}


////////////////////////////////////////////////////////////////////////////////

//RadioNode

//for(int i=channel; i<=channel+channels; i++){
//    m_active_channels[i] = trx_off;
//}


////////////////////////////////////////////////////////////////////////////////


WiFiNode::WiFiNode(QObject* parent, QString name, QVector3D pos, channel chan, bool class_n, RadPatternSet* antenna) :
    Transceiver(parent, name, pos)
  , m_power(10)
  , m_attenuation(-10)
  , m_duty(0.2)
  , m_channel(chan)
  , m_class_n(class_n)
  , m_average_tx_interval(m_slot_time / m_duty)
  , m_last_tx_time(0.0)
  , m_state(reset_state)
  , m_rotation()
  , m_antenna(antenna)
{
    m_next_event_time = m_difs_time;
}


void WiFiNode::timeout_callback(void)
{
    switch(m_state){
        case reset_state:{
        m_next_event_time = m_slot_time / m_duty;
        break;
    }

    }//switch

    m_next_event_time += m_difs_time;
}

void WiFiNode::reset(void)
{
    m_state = reset_state;
    m_next_event_time = m_slot_time / m_duty;

    foreach(auto chan, m_active_channels.keys()){
        m_active_channels[chan] = trx_off;
    }
}


////////////////////////////////////////////////////////////////////////////////

Radios::Radios(QObject* parent, double timeout) : QObject(parent)
  ,m_timeout(timeout)
  ,m_running(true)
  ,m_remote(NULL)
{

}

Radios::~Radios()
{
    qDeleteAll(m_trxs);
    m_trxs.clear();
}

void Radios::event_pump()
{
    if(!m_running)
        return;

    Transceiver* trx = NULL;
    double min_time = m_timeout;

    foreach(auto t, m_trxs){
        if(t->m_next_event_time < min_time){
            trx = t;
            timeout = false;
        }
    }
    if(trx == NULL){
        m_running = false;
        return;
    }

    trx->timeout_callback();
}

void Radios::state_change_callback(Transceiver *trx)
{
    Q_ASSERT(trx != NULL);
    //Run callback and
    Transceiver::trx_state trx_state = trx->get_state();
    Transceiver::trx_state last_trx_state = trx->get_last_state();

    bool update_tx = false;
    bool update_rx = false;
    switch(trx_state){
    case Transceiver::rx_on:{
        update_rx = true;
        if(last_trx_state == Transceiver::tx_on) update_tx = true;
        break;
    }
    case Transceiver::tx_on:{
        update_tx = true;
        break;
    }
    }//switch

    if(update_tx){
        trx->
    }

    if(update_rx){

        QMap<QString, QMap<int, QVector3D> > eirp_map;
        foreach(auto t, m_trxs){
            if(t->m_name != trx->m_name){
                QMap<int, QVector3D> chan_power;
                t->get_eirp(trx, &chan_power);
                eirp_map[t->m_name] = chan_power;
            }
        }
        trx->m_
    }
}

void Radios::add_trx(Transceiver* trx)
{
    trx->set_state_change_callback(&state_change_callback);
    m_trxs.append(trx);
}

void Radios::get_noise_power(Transceiver &rx, QMap<int, QVector3D>& chan_power)
{
    QVector3D rx_power;

}

////////////////////////////////////////////////////////////////////////////////
