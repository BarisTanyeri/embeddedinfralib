#ifndef PROTOBUF_PROTO_C_ECHO_PLUGIN_HPP
#define PROTOBUF_PROTO_C_ECHO_PLUGIN_HPP

#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/io/printer.h"
#include "protobuf/protoc_echo_plugin/CppFormatter.hpp"
#include "protobuf/protoc_echo_plugin/EchoObjects.hpp"

namespace application
{
    class EchoGenerator;

    class CppInfraCodeGenerator
        : public google::protobuf::compiler::CodeGenerator
    {
    public:
        virtual bool Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter,
            google::protobuf::compiler::GeneratorContext* generatorContext, std::string* error) const override;
    };

    class EnumGenerator
    {
    public:
        EnumGenerator(const std::shared_ptr<const EchoEnum>& enum_);
        EnumGenerator(const EnumGenerator& other) = delete;
        EnumGenerator& operator=(const EnumGenerator& other) = delete;
        ~EnumGenerator() = default;

        void Run(Entities& formatter);

    private:
        std::shared_ptr<const EchoEnum> enum_;
    };

    class MessageEnumGenerator
    {
    public:
        MessageEnumGenerator(const std::shared_ptr<const EchoMessage>& message);
        MessageEnumGenerator(const MessageEnumGenerator& other) = delete;
        MessageEnumGenerator& operator=(const MessageEnumGenerator& other) = delete;
        ~MessageEnumGenerator() = default;

        void Run(Entities& formatter);

    private:
        std::shared_ptr<const EchoMessage> message;
    };

    class MessageTypeMapGenerator
    {
    public:
        MessageTypeMapGenerator(const std::shared_ptr<const EchoMessage>& message, const std::string& prefix);
        MessageTypeMapGenerator(const MessageTypeMapGenerator& other) = delete;
        MessageTypeMapGenerator& operator=(const MessageTypeMapGenerator& other) = delete;
        ~MessageTypeMapGenerator() = default;

        void Run(Entities& formatter);

    protected:
        virtual void AddTypeMapType(EchoField& field, Entities& entities);
        std::string MessageName() const;
        virtual std::string MessageSuffix() const;

    protected:
        std::shared_ptr<const EchoMessage> message;
        std::string prefix;
    };

    class MessageReferenceTypeMapGenerator
        : public MessageTypeMapGenerator
    {
    public:
        using MessageTypeMapGenerator::MessageTypeMapGenerator;

        void Run(Entities& formatter);

    protected:
        virtual void AddTypeMapType(EchoField& field, Entities& entities) override;
        virtual std::string MessageSuffix() const override;
    };

    class MessageGenerator
    {
    public:
        MessageGenerator(const std::shared_ptr<const EchoMessage>& message, const std::string& prefix);
        MessageGenerator(const MessageGenerator& other) = delete;
        MessageGenerator& operator=(const MessageGenerator& other) = delete;
        ~MessageGenerator() = default;

        void Run(Entities& formatter);

    protected:
        virtual void GenerateTypeMap(Entities& formatter);
        void GenerateClass(Entities& formatter);
        virtual void GenerateConstructors();
        void GenerateFunctions();
        void GenerateTypeMap();
        virtual void GenerateGetters();
        void GenerateNestedMessageAliases();
        virtual void GenerateEnums();
        virtual void GenerateNestedMessages(Entities& formatter);
        virtual void GenerateFieldDeclarations();
        void GenerateFieldConstants();
        virtual void GenerateMaxMessageSize();
        virtual std::string SerializerBody();
        virtual std::string DeserializerBody();
        virtual std::string CompareEqualBody() const;
        virtual std::string CompareUnEqualBody() const;

        virtual std::string ClassName() const;
        virtual std::string ReferencedName() const;
        virtual std::string MessageSuffix() const;
        std::string TypeMapName() const;
        std::string ReferencedEnumPrefix() const;

    public:
        std::shared_ptr<const EchoMessage> message;

    protected:
        std::string prefix;
        Class* classFormatter;
    };

    class MessageReferenceGenerator
        : public MessageGenerator
    {
    public:
        using MessageGenerator::MessageGenerator;

    protected:
        virtual void GenerateTypeMap(Entities& formatter) override;
        virtual void GenerateConstructors() override;
        virtual void GenerateGetters() override;
        virtual void GenerateNestedMessages(Entities& formatter) override;
        virtual void GenerateFieldDeclarations() override;
        virtual void GenerateMaxMessageSize() override;
        virtual std::string SerializerBody() override;

        virtual std::string ClassName() const override;
        virtual std::string ReferencedName() const override;
        virtual std::string MessageSuffix() const override;
    };

    class ServiceGenerator
    {
    public:
        ServiceGenerator(const std::shared_ptr<const EchoService>& service, Entities& formatter);
        ServiceGenerator(const ServiceGenerator& other) = delete;
        ServiceGenerator& operator=(const ServiceGenerator& other) = delete;
        ~ServiceGenerator() = default;

    private:
        void GenerateServiceConstructors();
        void GenerateServiceProxyConstructors();
        void GenerateServiceFunctions();
        void GenerateServiceProxyFunctions();
        void GenerateFieldConstants();

        uint32_t MaxMessageSize() const;
        std::string HandleBody() const;
        std::string ProxyMethodBody(const EchoMethod& method) const;

    private:
        std::shared_ptr<const EchoService> service;
        Class* serviceFormatter;
        Class* serviceProxyFormatter;
    };

    class EchoGenerator
    {
    public:
        EchoGenerator(google::protobuf::compiler::GeneratorContext* generatorContext, const std::string& name, const google::protobuf::FileDescriptor* file);
        EchoGenerator(const EchoGenerator& other) = delete;
        EchoGenerator& operator=(const EchoGenerator& other) = delete;
        ~EchoGenerator() = default;

    public:
        void GenerateHeader();
        void GenerateSource();

    private:
        void GenerateTopHeaderGuard();
        void GenerateBottomHeaderGuard();

    private:
        google::protobuf::scoped_ptr<google::protobuf::io::ZeroCopyOutputStream> stream;
        google::protobuf::io::Printer printer;
        Entities formatter;
        const google::protobuf::FileDescriptor* file;

        std::vector<std::shared_ptr<EnumGenerator>> enumGenerators;
        std::vector<std::shared_ptr<MessageGenerator>> messageGenerators;
        std::vector<std::shared_ptr<MessageReferenceGenerator>> messageReferenceGenerators;
        std::vector<std::shared_ptr<ServiceGenerator>> serviceGenerators;
    };
}

#endif
