using System;
using System.Net;
using System.Windows.Forms;
using System.Xml;

namespace OpenWeatherMapAPI
{
    public partial class OpenWeatherMapAPIForm : Form
    {
        public OpenWeatherMapAPIForm()
        {
            InitializeComponent();
        }

        private const string API_KEY = "eb45f0434d91ea9bddfdb557d74efe82";

        private const string FORECAST_URL = "http://api.openweathermap.org/data/2.5/forecast?" + "q=@LOC@&mode=xml&units=imperial&APPID=" + API_KEY;

        private void btnForecast_Click(object sender, EventArgs e)
        {
            string url = FORECAST_URL.Replace("@LOC@", txtLocation.Text);

            XmlDocument xmlDocument;

            using (WebClient webClient = new WebClient())
            {
                //Get the response from the client and populate the string.
                string xml = webClient.DownloadString(url);

                //Load the response into a new XmlDocument.
                xmlDocument = new XmlDocument();
                xmlDocument.LoadXml(xml);
            }

            //Call ListTemperatures method with required argument.
            ListTemperatures(xmlDocument);
        }

        private void ListTemperatures(XmlDocument xmlDocument)
        {
            //Each time the method is called, clear any existing values.
            listViewValues.Items.Clear();

            string sameDayAsPreviousTime = "";

            //Loop through each Time node in the XmlDocument.
            foreach (XmlNode timeOfDayNode in xmlDocument.SelectNodes("//time"))
            {
                //Retrieve the Time attributes from each day
                XmlAttribute timeOfDayAttribute = timeOfDayNode.Attributes["from"];
                DateTime startingTime = DateTime.Parse(timeOfDayAttribute.Value);

                //Retrieve the Temperature attributes from each time in each day.
                XmlNode temperatureNode = timeOfDayNode.SelectSingleNode("temperature");
                XmlAttribute temperatureAttribute = temperatureNode.Attributes["value"];

                float temperature = 0;

                //If the Temperature attribute is not null, assign the value to the float.
                if (temperatureAttribute != null)
                {
                    temperature = float.Parse(temperatureAttribute.Value.ToString());
                }

                ListViewItem item;

                //If the same day, add a blank entry to the ListView
                if (startingTime.DayOfWeek.ToString() == sameDayAsPreviousTime)
                {
                    item = listViewValues.Items.Add("");
                }
                //If a new day, entry added to the ListView
                else
                {
                    sameDayAsPreviousTime = startingTime.DayOfWeek.ToString();
                    item = listViewValues.Items.Add(sameDayAsPreviousTime);
                }

                //Add the Time and Temperature as SubItems, so they appear in seperate columns.
                item.SubItems.Add(startingTime.ToShortTimeString());
                item.SubItems.Add(temperature.ToString("0.00"));
            }
        }
    }
}