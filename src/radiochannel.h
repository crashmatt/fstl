#ifndef RADIOCHANNEL_H
#define RADIOCHANNEL_H

#include <QQuaternion>
#include <QVector3D>

#include "radpatterndata.h"

////////////////////////////////////////////////////////////////////////////////

class Transceiver : public QObject
{
    Q_OBJECT
public:
    typedef int channel;

    typedef enum{
        trx_off,
        tx_on,
        rx_on
    } trx_state;

    Transceiver(QObject* parent, QString name, QVector3D pos);

    QString     m_name;
    QVector3D   m_pos;
    double      m_next_event_time;

    /**
     * get list of active receive channels for this tranceiver
     * @param channels reference to a list in which to put the active channels
     */
    void get_active_rx(QList<channel>& channels);

    /**
     * get effective incident radiated power vector towards the given tranceiver
     * @param rx reciever
     * @param chan_eirp Reference to map. Map Contents constructed from active rx channels
     */
    void get_eirp(Transceiver *rx, QMap<channel, QVector3D>& chan_eirp);

    /** get the tranciever state */
    trx_state get_state() {return m_trx_state;}

    /** get the last tranciever state */
    trx_state get_last_state() {return m_last_trx_state;}

    virtual void rx_start(double rx_time) = 0;
    virtual void rx_stop() = 0;
    virtual void tx_start(double tx_time) = 0;

    /**
     * Set the state change callback
     * @param callback pointer to callback function
     */
    void set_state_change_callback(void (*callback) (Transceiver*)) {state_change_callback=callback;}

protected:

    /** callback from event pump when timer is done. */
    virtual void timeout_callback(void) = 0;

    /** Set back to reset state */
    virtual void reset(void) = 0;

    /**
     * set the tranciever state
     * @param state new state
     */
    void set_state(trx_state state) {
        if(state != m_trx_state){
            m_last_trx_state=m_trx_state;
            m_trx_state=state;
            if(state_change_callback != NULL){
                state_change_callback(this);
            }
        }
    }


    /**
     * Map of eirp towards this Transceiver with tranmitter name referenced
     * Enables tracking of incident power on the receiver.
     */
    QMap <QString, QMap<channel, QVector3D> > m_trx_eirp_map;

    /** Tranceiver state */
    trx_state   m_trx_state;

    /** Previous Tranceiver state */
    trx_state   m_last_trx_state;

    void (*state_change_callback) (Transceiver*);
};


////////////////////////////////////////////////////////////////////////////////

class CSMANode : public Transceiver
{
    Q_OBJECT
public:
    typedef enum{
        csma_stop,
        csma_retry,
    } csma_state;

    CSMANode(QObject* parent, QString name, QVector3D pos);

    void csma_start();
    virtual bool csma_done();

    const double  m_backoff_level;
    const int     m_max_retries;
    const double  m_slot_time;
    const int     m_max_backoff_exponent;

protected:
    int     m_backoff_exponent;
};


////////////////////////////////////////////////////////////////////////////////


class RadioNode : public Transceiver
{
    Q_OBJECT
public:
//    typedef enum{
//        reset,
//        acquiring,
//        locked,
//    } node_state;

    RadioNode(QObject* parent, QString name, QVector3D pos, RadPatternSet* antenna);
    void set_remote(Transceiver* remote) {m_remote = remote;}

protected:
    Transceiver* m_remote;
};


////////////////////////////////////////////////////////////////////////////////

class WiFiNode : public Transceiver
{
    Q_OBJECT
public:
    typedef enum{
        reset_state,
        difs_wait,
        duty_wait,
        backoff_wait,
        transmitting,
    } wifi_state;

    WiFiNode(QObject* parent, QString name, QVector3D pos, channel chan, bool class_n, RadPatternSet* antenna);

    const double    m_power;
    const double    m_attenuation;
    const double    m_duty;
    const channel   m_channel;
    const bool      m_class_n;
    const double    m_difs_time;
    const double    m_slot_time;
    const double    m_average_tx_interval;
    double          m_last_tx_time;

    wifi_state      m_state;
    QQuaternion m_rotation;
    QExplicitlySharedDataPointer<RadPatternSet> m_antenna;

protected:
    virtual void timeout_callback(void);
    virtual void reset(void);
};

////////////////////////////////////////////////////////////////////////////////


class Radios : public QObject
{
    Q_OBJECT
public:
    Radios(QObject* parent, double timeout);
    ~Radios();

    void event_pump();
    void restart();
    void add_trx(Transceiver* trx);

protected:
    virtual bool timeout_callback(void);
    void state_change_callback(Transceiver *trx);
    virtual void reset(void);

    double  m_timeout;
    bool    m_running;
    QList<Transceiver*> m_trxs;
};

#endif // RADIOCHANNEL_H
