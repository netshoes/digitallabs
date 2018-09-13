using Moq;
using NUnit.Framework;
using BotTemplate.Dialogs;
using System.Collections.Generic;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;
using BotTemplate.Core.Dialogs;

namespace BotTemplate.App.Core.Tests
{
    [TestFixture]
    public class DialogIntentMapperTests
    {
        [Test]
        public void EmptyIntentList()
        {
            var mapper = new Mock<IIntentMapper>();

            var diag = new DialogIntentMapper(mapper.Object);

            var result = diag.GetDialogForIntent(new NaturalLanguageResult()
            {
                Entities = new List<NaturalLanguageEntity>(),
                Intents = new List<NaturalLanguageIntent>()
            });

            Assert.IsTrue(result.GetType() == typeof(NoneDialog));
        }

        [Test]
        public void NoIntentMapperFound()
        {
            var mapper = new Mock<IIntentMapper>();
            mapper.Setup(x => x.FindCanHandleIntent(It.IsAny<NaturalLanguageIntent>())).Returns(It.IsAny<InitialIntent>());

            var diag = new DialogIntentMapper(mapper.Object);

            var result = diag.GetDialogForIntent(new NaturalLanguageResult()
            {
                Entities = new List<NaturalLanguageEntity>(),
                Intents = new List<NaturalLanguageIntent>() { new NaturalLanguageIntent() { Intent = "Greetings" } }
            });

            Assert.IsTrue(result.GetType() == typeof(NoneDialog));
        }
    }
}
