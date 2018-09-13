using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Dialogs;
using System.Net;
using System.IO;
using Newtonsoft.Json;
using Microsoft.Bot.Connector;
using BotTemplate.Core.NaturalLanguage;
using System.Linq;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class FaleEsporteDialog : IDialog
    {
        const string accessKey = "c3d513c8d1e240ab820948f40ba69d0e";
        
        const string uriBase = "https://api.cognitive.microsoft.com/bing/v7.0/news/search";
        private List<NaturalLanguageEntity> entities;

        public FaleEsporteDialog(List<Core.NaturalLanguage.NaturalLanguageEntity> entities)
        {
            this.entities = entities;
        }

        struct SearchResult
        {
            public String jsonResult;
            public Dictionary<String, String> relevantHeaders;
        }

        public async Task StartAsync(IDialogContext context)
        {
            await this.DisplayThumbnailCard(context);
        }


        public async Task DisplayThumbnailCard(IDialogContext context)
        {
            try
            {
                var esporte = this.entities.First().Entity;
                var buscas = JsonConvert.DeserializeObject<RootObject>(BingNewsSearch(esporte).jsonResult);
                var reply = context.MakeMessage();
                
                reply.AttachmentLayout = AttachmentLayoutTypes.Carousel;
                reply.Attachments = GetCardsAttachments(buscas.value, esporte);

                if (reply.Attachments.Count == 0) throw new Exception();

                await context.PostAsync($"Então vamos ver algumas notícias");
                await context.PostAsync(reply);
            }
            catch (Exception)
            {
                await context.PostAsync($"Infelizmente não sei falar disso");
            }

            context.Done("");
        }

        private static IList<Attachment> GetCardsAttachments(List<Value> values, string esporte)
        {
            var lista = new List<Attachment>();

            foreach (var item in values)
            {
                try
                {
                    if(item.category.Equals("Sports"))
                        lista.Add(GetThumbnailCard(item.name, esporte, item.description, new CardImage(url: item.image.thumbnail.contentUrl), new CardAction(ActionTypes.OpenUrl, "Leia mais", value: item.url)));
                }catch(Exception)
                {
                    //Nada
                }
            }

            return lista;
        }
 

        private static Attachment GetThumbnailCard(string title, string subtitle, string text, CardImage cardImage, CardAction cardAction)
        {
            var heroCard = new ThumbnailCard
            {
                Title = title,
                Subtitle = subtitle,
                Text = text,
                Images = new List<CardImage>() { cardImage },
                Buttons = new List<CardAction>() { cardAction },
            };

            return heroCard.ToAttachment();
        }

        static SearchResult BingNewsSearch(string searchQuery)
        {
            // Construct the URI of the search request
            var uriQuery = uriBase + "?q=" + Uri.EscapeDataString(searchQuery);

            // Perform the Web request and get the response
            WebRequest request = HttpWebRequest.Create(uriQuery);
            request.Headers["Ocp-Apim-Subscription-Key"] = accessKey;
            request.Headers["BingAPIs-Market"] = "pt-BR";
            request.Headers["Accept-Language"] = "pt-BR";
            request.Headers["X-MSEdge-ClientIP"] = "179.190.26.98";
            HttpWebResponse response = (HttpWebResponse)request.GetResponseAsync().Result;
            string json = new StreamReader(response.GetResponseStream()).ReadToEnd();

            // Create result object for return
            var searchResult = new SearchResult();
            searchResult.jsonResult = json;
            searchResult.relevantHeaders = new Dictionary<String, String>();

            // Extract Bing HTTP headers
            foreach (String header in response.Headers)
            {
                if (header.StartsWith("BingAPIs-") || header.StartsWith("X-MSEdge-"))
                    searchResult.relevantHeaders[header] = response.Headers[header];
            }

            return searchResult;
        }



    }

    #region Classes

    public class QueryContext
    {
        public string originalQuery { get; set; }
        public bool adultIntent { get; set; }
    }

    public class Sort
    {
        public string name { get; set; }
        public string id { get; set; }
        public bool isSelected { get; set; }
        public string url { get; set; }
    }

    public class Thumbnail
    {
        public string contentUrl { get; set; }
        public int width { get; set; }
        public int height { get; set; }
    }

    public class Image
    {
        public Thumbnail thumbnail { get; set; }
    }

    public class About
    {
        public string readLink { get; set; }
        public string name { get; set; }
    }

    public class Provider
    {
        public string _type { get; set; }
        public string name { get; set; }
    }

    public class Value
    {
        public string name { get; set; }
        public string url { get; set; }
        public Image image { get; set; }
        public string description { get; set; }
        public List<About> about { get; set; }
        public List<Provider> provider { get; set; }
        public DateTime datePublished { get; set; }
        public string category { get; set; }
    }

    public class RootObject
    {
        public string _type { get; set; }
        public string readLink { get; set; }
        public QueryContext queryContext { get; set; }
        public int totalEstimatedMatches { get; set; }
        public List<Sort> sort { get; set; }
        public List<Value> value { get; set; }
    } 
    #endregion

}