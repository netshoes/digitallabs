using Autofac;
using System.Net.Http;
using System.Web.Http;
using System.Threading.Tasks;
using Microsoft.Bot.Connector;
using BotTemplate.Core.Dialogs;
using Microsoft.Bot.Builder.Dialogs;
using System.Web.Http.Description;
using System;

namespace BotTemplate
{
    [BotAuthentication]
    public class MessagesController : ApiController
    {
        [ResponseType(typeof(void))]
        public virtual async Task<HttpResponseMessage> Post([FromBody] Activity activity)
        {
            try
            {
                if (activity != null)
                {
                    switch (activity.GetActivityType())
                    {
                        case ActivityTypes.Message:
                            await Conversation.SendAsync(activity, () => Conversation.Container.Resolve<IRootDialog>());
                            break;

                        case ActivityTypes.ConversationUpdate:
                            IConversationUpdateActivity update = activity as IConversationUpdateActivity;
                            await StartProactiveMessage(activity, update);
                            break;
                    }
                }
                return new HttpResponseMessage(System.Net.HttpStatusCode.Accepted);
            }
            catch(Exception e)
            {
                throw e;
            }
                
        }

        private static async Task StartProactiveMessage(Activity activity, IConversationUpdateActivity iConversationUpdated)
        {
            if (iConversationUpdated != null)
            {
                ConnectorClient connector = new ConnectorClient(new System.Uri(iConversationUpdated.ServiceUrl));

                foreach (var member in iConversationUpdated.MembersAdded ?? System.Array.Empty<ChannelAccount>())
                {
                    if (member.Id == iConversationUpdated.Recipient.Id)
                    {
                        await Conversation.SendAsync(activity, () => Conversation.Container.Resolve<IRootDialog>());
                    }
                }
            }
        }
    }
}