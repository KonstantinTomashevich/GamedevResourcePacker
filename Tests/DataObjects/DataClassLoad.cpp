#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <Plugins/DataObjects/DataClass.hpp>

using namespace GamedevResourcePacker::DataObjectsPlugin;

BOOST_AUTO_TEST_CASE(DataClassLoadCorrect)
{
    boost::unit_test::unit_test_log.set_threshold_level (boost::unit_test::log_level::log_messages);
    BOOST_TEST_MESSAGE ("Launching test case DataClassLoadCorrect.");
    auto &masterSuite = boost::unit_test::framework::master_test_suite ();
    boost::filesystem::path xmlPath (masterSuite.argv[masterSuite.argc - 1]);
    xmlPath /= "DataClassLoad";
    xmlPath /= "CorrectClass.xml";

    BOOST_TEST_MESSAGE ("Given file path: " << xmlPath.string ());
    DataClass dataClass (xmlPath);

    BOOST_TEST_MESSAGE ("Class name: " << dataClass.GetName ());
    BOOST_TEST_CHECK (dataClass.GetName () == "Character");
    const DataClass::FieldVector &fields = dataClass.GetFields ();

    DataClass::Field expectedFields[] = {
        {"x", "float", false, false, false},
        {"y", "float", false, false, false},
        {"health", "float", false, false, false},
        {"equipment", "EquipmentItem", true, false, true},
        {"archetype", "Archetype", false, true, true},
    };

    BOOST_TEST_MESSAGE ("Fields count: " << fields.size ());
    BOOST_TEST_CHECK (fields.size () == 5);

    for (int32_t index = 0; index < fields.size (); ++index)
    {
        DataClass::Field field = fields[index];
        BOOST_TEST_MESSAGE ("Field #" << index << ": " << (field.readonly ? "readonly" : "readwrite") <<
                                      " " << (field.array ? "array" : "value") << " " << field.name << " of "
                                      << field.typeName <<
                                      (field.reference ? "@." : "."));

        BOOST_TEST_CHECK (field.name == expectedFields[index].name);
        BOOST_TEST_CHECK (field.typeName == expectedFields[index].typeName);
        BOOST_TEST_CHECK (field.array == expectedFields[index].array);
        BOOST_TEST_CHECK (field.reference == expectedFields[index].reference);
        BOOST_TEST_CHECK (field.readonly == expectedFields[index].readonly);
    }
}
