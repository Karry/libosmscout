#include <osmscout/SearchLocationModel.h>
using namespace osmscout;

void sortAndPrint(const QList<LocationEntry> foundLocationsConst) {
  QList<LocationEntry> locations = foundLocationsConst;

  int i=0;
  for (auto it=locations.begin(); it!=locations.end(); ++i) {
    if (i%2==0) {
      it=locations.erase(it);
    } else {
      ++it;
    }
  }

  auto Compare = [&](const LocationEntry &a, const LocationEntry &b) -> bool {
    assert(a.parent() == nullptr && b.parent() == nullptr);
    return a.getLabel() < b.getLabel();
  };

  std::sort(locations.begin(), locations.end(), Compare);

  for (auto const &l:locations){
    qDebug() << l.getLabel();
  }
}

int main() {
  QList<LocationEntry> locations;

  uint64_t hash = 12345;
  for (int i=0; i<100000; ++i){
    locations << LocationEntry(QString("%1").arg(hash), GeoCoord(0,0));
    hash += (hash << 8);
  }

  sortAndPrint(locations);

  return 0;
}