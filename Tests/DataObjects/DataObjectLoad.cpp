#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <sstream>
#include <Plugins/DataObjects/DataClassProvider.hpp>
#include <Plugins/DataObjects/DataObject.hpp>

using namespace GamedevResourcePacker::DataObjectsPlugin;

BOOST_AUTO_TEST_CASE(DataObjectLoadCorect)
{
    boost::unit_test::unit_test_log.set_threshold_level (boost::unit_test::log_level::log_messages);
    BOOST_TEST_MESSAGE ("Launching test case DataObjectLoadCorrect.");
    auto &masterSuite = boost::unit_test::framework::master_test_suite ();
    boost::filesystem::path xmlPath (masterSuite.argv[masterSuite.argc - 1]);
    xmlPath /= "DataObjectLoad";

    DataClassProvider provider;
    BOOST_TEST_MESSAGE ("Loading CoreClass...");
    provider.AddDataClass (new DataClass(xmlPath / "CoreClass.xml"));
    BOOST_TEST_MESSAGE ("Loading AdditionalClass...");
    provider.AddDataClass (new DataClass(xmlPath / "AdditionalClass.xml"));

    try
    {
        BOOST_TEST_MESSAGE ("Loading Object xml file...");
        boost::property_tree::ptree tree;
        boost::filesystem::path rootAsset = xmlPath / "Object.xml";
        boost::property_tree::read_xml (rootAsset.string (), tree);
        auto xmlRoot = tree.get_child ("data-object");

        auto name = xmlRoot.get_child ("<xmlattr>.name");
        boost::property_tree::ptree rootObject;
        std::string rootObjectName;

        for (auto child : xmlRoot)
        {
            if (child.first != "<xmlattr>")
            {
                rootObject = child.second;
                rootObjectName = child.first;
                break;
            }
        }

        BOOST_TEST_MESSAGE ("Parsing object...");
        DataObject object (name.data (), rootAsset, rootObjectName, rootObject, nullptr, &provider);

        BOOST_TEST_MESSAGE ("Converting object to string...");
        std::ostringstream osstream;
        object.Print (osstream);
        std::string result = osstream.str ();

        BOOST_TEST_MESSAGE ("Result (size = ") << std::to_string(result.size ()) << ").";
        BOOST_TEST_MESSAGE (result);

        std::string expected;
        boost::filesystem::load_string_file (xmlPath / "Expected.txt", expected);

        BOOST_TEST_MESSAGE ("Expected (size = ") << std::to_string(expected.size ()) << ").";
        BOOST_TEST_MESSAGE (expected);
        BOOST_CHECK_EQUAL (result, expected);
    }
    catch (boost::exception &exception)
    {
        BOOST_TEST_MESSAGE ("Exception caught: ") << boost::diagnostic_information (exception);
        BOOST_FAIL ("Unable to parse test data because of the exception.");
    }
}
