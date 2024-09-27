#include "settings.h"

// most of this code is from https://github.com/supercollider/supercollider
// specifically the YAML parsing and writing functions

static QVariant parseScalar(const YAML::Node& node) {
    using namespace YAML;

    switch (node.Type()) {
    case NodeType::Scalar: {
        std::string val = node.as<std::string>();
        return QVariant(QString::fromUtf8(val.c_str()));
    }

    case NodeType::Sequence: {
        QVariantList list;
        for (auto const& element : node)
            list.append(parseScalar(element));
        return QVariant::fromValue<QVariantList>(list);
    }

    case NodeType::Map: {
        QVariantMap map;
        for (auto const& element : node) {
            std::string key = element.first.as<std::string>();
            QVariant value = parseScalar(element.second);
            map.insert(QString(key.c_str()), value);
        }
        return QVariant::fromValue<QVariantMap>(map);
    }

    case NodeType::Null:
        return QVariant();

    default:
        qWarning("YAML parsing: unsupported node type.");
        return QVariant();
    }
}


static void parseNode(const YAML::Node& node, const QString& parentKey, QSettings::SettingsMap& map) {
    using namespace YAML;

    static const std::string qVariantListTag("!QVariantList");
    static const std::string qVariantMapTag("!QVariantMap");

    Q_ASSERT(node.Type() == NodeType::Map);
    for (auto const& element : node) {
        std::string key = element.first.as<std::string>();
        QString childKey(parentKey);
        if (!childKey.isEmpty())
            childKey += "/";
        childKey += key.c_str();

        const YAML::Node& childNode = element.second;
        const std::string& childTag = childNode.Tag();

        // if (childTag == qVariantListTag || childTag == qVariantMapTag || childNode.Type() != NodeType::Map)
        if (childNode.Type() == NodeType::Map)
            parseNode(childNode, childKey, map);
        else
            map.insert(childKey, parseScalar(childNode));
    }
}

bool readSettings(QIODevice& device, QSettings::SettingsMap& map) {
    using namespace YAML;

    try {
        Node doc = Load(device.readAll().toStdString());
        if (doc) {
            if (doc.IsMap()) {
                QString key;
                parseNode(doc, key, map);
            }
        }

        return true;
    } catch (std::exception& e) {
        qWarning() << "Exception when parsing YAML config file:" << e.what();
        qErrnoWarning("Exception when parsing YAML config file. Check error logs for more information");
        return false;
    }
}

static void writeValue(const QVariant& var, YAML::Emitter& out) {
    // this code is deprecated

    switch (var.type()) {
    case QVariant::Invalid: {
        out << YAML::Null;
        break;
    }
    case QVariant::KeySequence: {
        QKeySequence kseq = var.value<QKeySequence>();

        out << kseq.toString(QKeySequence::PortableText).toUtf8().constData();

        break;
    }
    case QVariant::List: {
        out << YAML::LocalTag("QVariantList") << YAML::BeginSeq;

        QVariantList list = var.value<QVariantList>();
        foreach (const QVariant& var, list)
            writeValue(var, out);

        out << YAML::EndSeq;

        break;
    }
    case QVariant::Map: {
        out << YAML::LocalTag("QVariantMap") << YAML::BeginMap;

        QVariantMap map = var.value<QVariantMap>();
        QVariantMap::iterator it;
        for (it = map.begin(); it != map.end(); ++it) {
            out << YAML::Key << it.key().toStdString();
            out << YAML::Value;
            writeValue(it.value(), out);
        }

        out << YAML::EndMap;

        break;
    }
    case QVariant::UserType: {
        int utype = var.userType();
        out << var.toString().toUtf8().constData();
        break;
    }
    default: {
        out << var.toString().toUtf8().constData();
    }
    }
}

static void writeGroup(const QString& groupKey, YAML::Emitter& out, SettingsIterator& it,
                       const QSettings::SettingsMap& map) {
    out << YAML::BeginMap;

    int groupKeyLen = groupKey.size();

    while (it != map.end()) {
        QString key(it.key());

        if (!key.startsWith(groupKey))
            break;

        int i_separ = key.indexOf("/", groupKeyLen);
        if (i_separ != -1) {
            // There is child nodes
            key.truncate(i_separ + 1);

            QString yamlKey(key);
            yamlKey.remove(0, groupKeyLen);
            yamlKey.chop(1);

            out << YAML::Key << yamlKey.toStdString();
            out << YAML::Value;

            writeGroup(key, out, it, map);
        } else {
            // There is no child nodes
            key.remove(0, groupKeyLen);

            out << YAML::Key << key.toStdString();
            out << YAML::Value;

            writeValue(it.value(), out);

            ++it;
        }
    }

    out << YAML::EndMap;
}

bool writeSettings(QIODevice& device, const QSettings::SettingsMap& map) {
    try {
        YAML::Emitter out;
        SettingsIterator it = map.begin();
        writeGroup("", out, it, map);
        device.write(out.c_str());
        return true;
    } catch (std::exception& e) {
        qWarning() << "Exception when writing YAML config file:" << e.what();
        qErrnoWarning("Exception when writing YAML config file. Check error logs for more information");
        return false;
    }
}

QSettings::Format registerYAMLFormat() {
    static QSettings::Format format = QSettings::registerFormat("yaml", readSettings, writeSettings);

    if (format == QSettings::InvalidFormat)
        qWarning("Could not register settings format");

    return format;
}
