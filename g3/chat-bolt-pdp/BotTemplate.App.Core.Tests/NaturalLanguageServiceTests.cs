using Moq;
using NUnit.Framework;
using System.Collections.Generic;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.App.Core.Tests
{
    [TestFixture]
    public class NaturalLanguageServiceTests
    {
        [Test]
        public void InsureReturnedResultsAreNotNull()
        {
            var nlServices = new Mock<IEnumerable<INaturalLanguageService>>();

        }

    }
}
