/*  ArucoMarkerTracker
    Copyright (C) 2021 Kuppens Brecht

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "TestSourceCode.h"
#include "TestFactory.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

REGISTER_TESTCLASS(TestSourceCode);

const QSet<QString> cStyleCommentSuffixes({ "cpp", "h", "qml" });
const QSet<QString> hashtagStyleCommentSuffixes({ "pro", "pri" });
const QSet<QString> suffixesToCheck = cStyleCommentSuffixes + hashtagStyleCommentSuffixes;

void TestSourceCode::source_code_must_contain_license_header()
{
    QFile headerTemplateCStyleFile("://HeaderTemplateCStyle.txt");
    QVERIFY(headerTemplateCStyleFile.open(QIODevice::ReadOnly));
    QByteArray headerCStyle = headerTemplateCStyleFile.readAll().simplified();
    QVERIFY(headerCStyle.length() > 0);

    QFile headerTemplateHashtagStyleFile("://HeaderTemplateHashtagStyle.txt");
    QVERIFY(headerTemplateHashtagStyleFile.open(QIODevice::ReadOnly));
    QByteArray headerHashtagStyle = headerTemplateHashtagStyleFile.readAll().simplified();
    QVERIFY(headerHashtagStyle.length() > 0);

    QDir rootPath(QDir(QFileInfo(__FILE__).absolutePath()).absoluteFilePath(".."));
    QDirIterator dirIterator(rootPath.absolutePath(), QDirIterator::Subdirectories);
    QStringList badFiles;
    while (dirIterator.hasNext()) {
        QDir path(dirIterator.next());
        foreach (QString fileName, path.entryList(QStringList(), QDir::Files | QDir::Readable)) {
            QString fullFileName = path.absoluteFilePath(fileName);
            QFileInfo fileInfo(fullFileName);

            if (suffixesToCheck.contains(fileInfo.suffix())) {
                QFile f(fullFileName);
                QVERIFY(f.open(QIODevice::ReadOnly));
                QByteArray content = f.readAll().simplified();
                QVERIFY(content.length() > 0);

                if (cStyleCommentSuffixes.contains(fileInfo.suffix()) && !content.startsWith(headerCStyle)) {
                    badFiles << fullFileName;
                }
                if (hashtagStyleCommentSuffixes.contains(fileInfo.suffix()) && !content.startsWith(headerHashtagStyle)) {
                    badFiles << fullFileName;
                }
            }
        }
    }
    if (!badFiles.empty()) {
        QFAIL(qPrintable(badFiles.join(" ")));
    }
}
