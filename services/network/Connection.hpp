#ifndef SERVICES_CONNECTION_HPP
#define SERVICES_CONNECTION_HPP

#include "infra/stream/InputStream.hpp"
#include "infra/stream/OutputStream.hpp"
#include "infra/util/AutoResetFunction.hpp"
#include "infra/util/IntrusiveList.hpp"
#include "infra/util/SharedOwnedObserver.hpp"
#include "services/network/Address.hpp"

namespace services
{
    class Connection;

    class ConnectionObserver
        : public infra::SharedOwnedObserver<ConnectionObserver, Connection>
    {
    public:
        virtual void SendStreamAvailable(infra::SharedPtr<infra::StreamWriter>&& streamWriter) = 0;
        virtual void DataReceived() = 0;
        virtual void Close();
        virtual void Abort();

    private:
        friend class Connection;
    };

    class Connection
        : public infra::SharedOwningSubject<ConnectionObserver>
    {
    public:
        // A new send stream may only be requested when any previous send stream has been destroyed
        virtual void RequestSendStream(std::size_t sendSize) = 0;
        virtual std::size_t MaxSendStreamSize() const = 0;
        // A new receive stream may only be requested when any previous receive streams has been destroyed
        virtual infra::SharedPtr<infra::StreamReaderWithRewinding> ReceiveStream() = 0;
        // When data from the receive stream has been processed, call AckReceived to free the TCP window
        // If AckReceived is not called, a next call to ReceiveStream will return a stream consisting of the same data
        virtual void AckReceived() = 0;

        virtual void CloseAndDestroy() = 0;
        virtual void AbortAndDestroy() = 0;
    };

    class ConnectionWithHostname
        : public Connection
    {
    public:
        virtual void SetHostname(infra::BoundedConstString hostname) = 0;
    };

    class ServerConnectionObserverFactory
    {
    protected:
        ServerConnectionObserverFactory() = default;
        ServerConnectionObserverFactory(const ServerConnectionObserverFactory& other) = delete;
        ServerConnectionObserverFactory& operator=(const ServerConnectionObserverFactory& other) = delete;
        ~ServerConnectionObserverFactory() = default;

    public:
        virtual void ConnectionAccepted(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver, IPAddress address) = 0;
    };

    class ClientConnectionObserverFactory
        : public infra::IntrusiveList<ClientConnectionObserverFactory>::NodeType
    {
    protected:
        ClientConnectionObserverFactory() = default;
        ClientConnectionObserverFactory(const ClientConnectionObserverFactory& other) = delete;
        ClientConnectionObserverFactory& operator=(const ClientConnectionObserverFactory& other) = delete;
        ~ClientConnectionObserverFactory() = default;

    public:
        enum ConnectFailReason
        {
            refused,
            connectionAllocationFailed
        };

        virtual IPAddress Address() const = 0;
        virtual uint16_t Port() const = 0;

        virtual void ConnectionEstablished(infra::AutoResetFunction<void(infra::SharedPtr<services::ConnectionObserver> connectionObserver)>&& createdObserver) = 0;
        virtual void ConnectionFailed(ConnectFailReason reason) = 0;
    };

    class ConnectionFactory
    {
    protected:
        ConnectionFactory() = default;
        ConnectionFactory(const ConnectionFactory& other) = delete;
        ConnectionFactory& operator=(const ConnectionFactory& other) = delete;
        ~ConnectionFactory() = default;

    public:
        virtual infra::SharedPtr<void> Listen(uint16_t port, ServerConnectionObserverFactory& factory, IPVersions versions = IPVersions::both) = 0;
        virtual void Connect(ClientConnectionObserverFactory& factory) = 0;
        virtual void CancelConnect(ClientConnectionObserverFactory& factory) = 0;
    };

    class ConnectionWithHostnameDecorator
        : public ConnectionWithHostname
        , public ConnectionObserver
    {
    public:
        // Implementation of Connection
        virtual void RequestSendStream(std::size_t sendSize) override;
        virtual std::size_t MaxSendStreamSize() const override;
        virtual infra::SharedPtr<infra::StreamReaderWithRewinding> ReceiveStream() override;
        virtual void AckReceived() override;
        virtual void CloseAndDestroy() override;
        virtual void AbortAndDestroy() override;
        virtual void SetHostname(infra::BoundedConstString hostname) override;

        void Attach(const infra::SharedPtr<ConnectionObserver>& observer);

        // Implementation of ConnectionObserver
        virtual void SendStreamAvailable(infra::SharedPtr<infra::StreamWriter>&& streamWriter) override;
        virtual void DataReceived() override;
        virtual void Detaching() override;
        virtual void Close() override;
        virtual void Abort() override;

    private:
        bool closing = false;
        bool aborting = false;
    };

    static const uint32_t ethernetMtu = 1500;
    static const uint32_t tcpPacketOverhead = 54;
    static const uint32_t maxPacketPayload = ethernetMtu - tcpPacketOverhead;
}

#endif
